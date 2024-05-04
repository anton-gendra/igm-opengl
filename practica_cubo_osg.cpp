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
   //pat->setDataVariance(osg::Object::DYNAMIC);
   pat->setPosition(translation);

   return pat;
}


class RotateCB : public osg::NodeCallback
{
public:
    RotateCB() : _angle(0.) {}
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::PositionAttitudeTransform* pat =
            dynamic_cast<osg::PositionAttitudeTransform*>(node);
        if (pat)
        {
            // New orientation
            osg::Quat newAttitude;
            newAttitude.makeRotate(_angle, osg::Vec3(0., 0., 1.)); // Rotatioin
            
            // Configure position
            pat->setPosition(osg::Vec3(-6., 0., 0.)); // Constant translationi
            pat->setAttitude(newAttitude);

            // Increase angle for next rotation
            _angle += 0.01;

            // Continue
            traverse(node, nv);
        }
    }

protected:
    double _angle;
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

   // Create the scene graph
   // const double translation = 2.2 * loadedModel->getBound().radius(); // (1)
   double currentTime = globalTimer.time_s();

   float f = (float)currentTime * 0.3f;
   // osg::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
   // translation = osg::translate(model, osg::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));
   osg::Vec3 translation = osg::Vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);
   std::cout << "Initial position: (" << translation.x() << ", " << translation.y() << ", " << translation.z() << ")\n";

   osg::ref_ptr<osg::Group> root(new osg::Group());

   osg::ref_ptr<osg::PositionAttitudeTransform> spinningCube =
       CreateSubGraph(root, loadedModel, translation);
   spinningCube->setDataVariance(osg::Object::DYNAMIC);
   spinningCube->setUpdateCallback(new RotateCB);


   // Play with the StateSets
   osg::ref_ptr<osg::StateSet> rootSS = root->getOrCreateStateSet();
   rootSS->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

   // Create a viewer, use it to view the model
   osgViewer::Viewer viewer;
   viewer.setSceneData(root);
   // Set background colour to black
   viewer.getCamera()->setClearColor(osg::Vec4(0.0f,0.0f,0.0f,0.0f));
   // Set the camera initial position
   osg::Vec3d eye(50.0, 50.0, 0.0);
   osg::Vec3d center(0.0, 0.0, 0.0);
   osg::Vec3d up(0.0, 1.0, 0.0);
   viewer.getCamera()->setViewMatrixAsLookAt(eye, center, up);

   // Enter rendering loop
   viewer.run();
}
