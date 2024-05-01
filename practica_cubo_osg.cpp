#include <cstring>
#include <iostream>
#include <osg/Group>
#include <osg/PolygonStipple>
#include <osg/PolygonMode>
#include <osg/PositionAttitudeTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#include <osg/Timer>

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

int main(int argc, char *argv[])
{
   // Check command-line parameters
   if (argc != 2)
   {
      std::cerr << "Usage: " << argv[0] << " <model file>\n";
      exit(1);
   }

   // Load the model
   osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(argv[1]);

   if (!loadedModel)
   {
      std::cerr << "Problem opening '" << argv[1] << "'\n";
      exit(1);
   }

   /*
      glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
      model = glm::translate(model, glm::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));
      model = glm::rotate(model, glm::radians((float)currentTime * 45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      model = glm::rotate(model, glm::radians((float)currentTime * 81.0f), glm::vec3(1.0f, 0.0f, 0.0f));
   */

   // Create the scene graph
   // const double translation = 2.2 * loadedModel->getBound().radius(); // (1)
   double currentTime = globalTimer.time_s();

   float f = (float)currentTime * 0.3f;
   // osg::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));
   // translation = osg::translate(model, osg::vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f));
   osg::Vec3 translation = osg::Vec3(sinf(2.1f * f) * 0.5f, cosf(1.7f * f) * 0.5f, sinf(1.3f * f) * cosf(1.5f * f) * 2.0f);

   osg::ref_ptr<osg::Group> root(new osg::Group());

   osg::ref_ptr<osg::PositionAttitudeTransform> spinningCube =
       CreateSubGraph(root, loadedModel, translation);


   // Play with the StateSets
   osg::ref_ptr<osg::StateSet> rootSS = root->getOrCreateStateSet();
   rootSS->setMode(GL_LIGHTING, osg::StateAttribute::OFF); // (2)

   // Create a viewer, use it to view the model
   osgViewer::Viewer viewer;
   viewer.setSceneData(root);

   // Enter rendering loop
   viewer.run();
}
