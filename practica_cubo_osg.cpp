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

class MoveAndRotateCB : public osg::NodeCallback
{
public:
    MoveAndRotateCB() {}

    virtual void operator()(osg::Node *node, osg::NodeVisitor *nv)
    {
        osg::PositionAttitudeTransform *pat = dynamic_cast<osg::PositionAttitudeTransform *>(node);
        if (pat)
        {
            // Get animation time
            // double currentTime = nv->getFrameStamp()->getSimulationTime();
            double currentTime = globalTimer.time_s();
            float f = (float)currentTime * 0.3f;

            float depth = sinf(1.3f * f) * cosf(1.5f * f) * 2.0f;
            // Initial tranlation and animated position
            osg::Vec3 initialPos(0.0f, 0.0f, -4.0f);
            osg::Vec3 animatedPos(
                sinf(2.1f * f) * 0.5f,
                depth,
                cosf(1.7f * f) * 0.5f);
            pat->setPosition(animatedPos);

            // Scale based on depth
            float scaleValue = 1.0f + (depth / 4.0f);
            osg::Vec3 scaleVec(scaleValue, scaleValue, scaleValue);
            pat->setScale(scaleVec);

            // Multiple axis combined rotations
            osg::Quat rotationY, rotationX;
            rotationY.makeRotate(osg::DegreesToRadians((float)currentTime * 45.0f), osg::Vec3(0.0f, 1.0f, 0.0f));
            rotationX.makeRotate(osg::DegreesToRadians((float)currentTime * 81.0f), osg::Vec3(1.0f, 0.0f, 0.0f));
            osg::Quat rotation = rotationX * rotationY;
            pat->setAttitude(rotation);

            // Continue
            traverse(node, nv);
        }
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
    std::cout << "Initial position: (" << translation.x() << ", " << translation.y() << ", " << translation.z() << ")\n";

    osg::ref_ptr<osg::Group> root(new osg::Group());

    osg::ref_ptr<osg::PositionAttitudeTransform> spinningCube =
        CreateSubGraph(root, loadedModel, translation);
    spinningCube->setDataVariance(osg::Object::DYNAMIC);
    spinningCube->setUpdateCallback(new MoveAndRotateCB);

    // Play with the StateSets
    osg::ref_ptr<osg::StateSet> rootSS = root->getOrCreateStateSet();
    rootSS->setMode(GL_LIGHTING, osg::StateAttribute::ON);

    // Create a viewer, use it to view the model
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);

    // Set background colour to black
    viewer.getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Enter rendering loop
    viewer.run();
}
