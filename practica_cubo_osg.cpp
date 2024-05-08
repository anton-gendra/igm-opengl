#include <cstring>
#include <iostream>
#include <osg/Group>
#include <osg/PolygonStipple>
#include <osg/PolygonMode>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#include <osg/Timer>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osg/TexGen>

// Crear un timer global
static osg::Timer globalTimer;

osg::ref_ptr<osg::PositionAttitudeTransform>
CreateSubGraph(osg::ref_ptr<osg::Group> root,
               osg::ref_ptr<osg::Node> model,
               osg::Vec3 translation)
{
    osg::ref_ptr<osg::PositionAttitudeTransform> pat(
        new osg::PositionAttitudeTransform());

    root->addChild(pat);
    pat->addChild(model);
    pat->setPosition(translation);

    return pat;
}

void performTranslation(osg::Node *node, osg::NodeVisitor *nv, osg::Vec3 initialPos, float desyncFactor)
{
    osg::PositionAttitudeTransform *pat = dynamic_cast<osg::PositionAttitudeTransform *>(node);
    if (pat)
    {
        // Get animation time
        // double currentTime = nv->getFrameStamp()->getSimulationTime();
        double currentTime = globalTimer.time_s();
        float f = (float)currentTime * 0.3f;

        float depth = sinf(1.3f * f) * cosf(1.5f * f) * 2.0f * desyncFactor;
        osg::Vec3 animatedPos(
            sinf(2.1f * f) * 0.5f * desyncFactor,
            depth,
            cosf(1.7f * f) * 0.5f * desyncFactor);
        pat->setPosition(initialPos + animatedPos);

        // Scale based on depth
        float scaleValue = 1.0f + (depth / 4.0f);
        osg::Vec3 scaleVec(scaleValue, scaleValue, scaleValue);
        pat->setScale(scaleVec);

        // Multiple axis combined rotations
        osg::Quat rotationY, rotationX;
        rotationY.makeRotate(osg::DegreesToRadians((float)currentTime * 45.0f * desyncFactor), osg::Vec3(0.0f, 1.0f, 0.0f));
        rotationX.makeRotate(osg::DegreesToRadians((float)currentTime * 81.0f * desyncFactor), osg::Vec3(1.0f, 0.0f, 0.0f));
        osg::Quat rotation = rotationX * rotationY;
        pat->setAttitude(rotation);
    }
}

class MoveAndRotateCB : public osg::NodeCallback
{
public:
    MoveAndRotateCB() {}

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
    {
        osg::Vec3 initialPos(-1.0f, 0.0f, 0.0f);
        performTranslation(node, nv, initialPos, 1.0);

        // Continue
        traverse(node, nv);
    }
};

class MoveAndRotateSecondCube : public osg::NodeCallback
{
public:
    MoveAndRotateSecondCube() {}

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
    {
        osg::Vec3 initialPos(1.0f, 2.5f, 0.0f);
        performTranslation(node, nv, initialPos, 1.25);

        // Continue
        traverse(node, nv);
    }
};

int main(int argc, char *argv[])
{
    // Load the model
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("cube.obj");

    if (!loadedModel)
    {
        std::cerr << "Problem opening 'cube.obj'\n";
        exit(1);
    }

    osg::Vec3 translation = osg::Vec3(0.0f, -5.5f, 0.0f);
    osg::Vec3 secondCubeTranslation = osg::Vec3(0.0f, -5.5f, 0.0f);
    std::cout << "1C Initial position: (" << translation.x() << ", " << translation.y() << ", " << translation.z() << ")\n";
    std::cout << "2C Initial position: (" << secondCubeTranslation.x() << ", " << secondCubeTranslation.y() << ", " << secondCubeTranslation.z() << ")\n";

    osg::ref_ptr<osg::Group> root(new osg::Group());

    osg::ref_ptr<osg::PositionAttitudeTransform> spinningCube =
        CreateSubGraph(root, loadedModel, translation);
    spinningCube->setDataVariance(osg::Object::DYNAMIC);
    spinningCube->setUpdateCallback(new MoveAndRotateCB);

    osg::ref_ptr<osg::PositionAttitudeTransform> secondSpinningCube =
        CreateSubGraph(root, loadedModel, secondCubeTranslation);
    secondSpinningCube->setDataVariance(osg::Object::DYNAMIC);
    secondSpinningCube->setUpdateCallback(new MoveAndRotateSecondCube);

    // Texture
    // Do the texturing stuff
    osg::ref_ptr<osg::StateSet> ss = loadedModel->getOrCreateStateSet();

    osg::ref_ptr<osg::Image> image = osgDB::readImageFile("texture.jpg"); // (1)
    osg::ref_ptr<osg::Texture2D> tex(new osg::Texture2D());               // (1)
    tex->setImage(image);                                                 // (1)
    ss->setTextureAttributeAndModes(0, tex);                              // (1)

    osg::ref_ptr<osg::TexGen> texGen(new osg::TexGen());                // (2)
    texGen->setPlane(osg::TexGen::S, osg::Plane(1.0, 1.0, 0.0, 0.0)); // (2)
    texGen->setPlane(osg::TexGen::T, osg::Plane(0.0, 1.0, 1.0, 0.0)); // (2)
    ss->setTextureAttributeAndModes(0, texGen);                         // (2)

    // Light
    osg::ref_ptr<osg::PositionAttitudeTransform> lightPAT(
        new osg::PositionAttitudeTransform());

    lightPAT->setPosition(osg::Vec3(0.0, 0.0, 0.0));
    lightPAT->setScale(osg::Vec3(0.25f, 0.25f, 0.25f));
    root->addChild(lightPAT);

    // Setup GL_LIGHT1. Leave GL_LIGHT0 as it is by default (enabled)
    osg::ref_ptr<osg::LightSource> lightSource(new osg::LightSource());
    lightSource->addChild(loadedModel);
    lightSource->getLight()->setLightNum(1);
    lightSource->getLight()->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0));
    lightSource->getLight()->setDiffuse(osg::Vec4(1.0, 1.0, 0.0, 1.0));
    lightPAT->addChild(lightSource);

    // Play with the StateSets
    osg::ref_ptr<osg::StateSet> rootSS = root->getOrCreateStateSet();
    rootSS->setMode(GL_LIGHT1, osg::StateAttribute::ON);
    rootSS->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    // Create a viewer, use it to view the model
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);

    // Set background colour to black
    viewer.getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Enter rendering loop
    viewer.run();
}
