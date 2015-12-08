#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/Group>
#include <osgFX/Scribe>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>
#include <osg/MatrixTransform>
#include "Pick.h"


int main()
{
	osg::ref_ptr<osgViewer::Viewer> myViewer= new osgViewer::Viewer;
	osg::ref_ptr<osg::Group> root= new osg::Group;
	osg::ref_ptr<osg::Node> myCow=osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cow.osg");
    osg::ref_ptr<osg::MatrixTransform> mt= new osg::MatrixTransform;
	osg::ref_ptr<osg::Node> myCessna= osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cessna.osg");
	mt->addChild(myCessna);
	mt->setMatrix(osg::Matrix::translate(20,0,0));
	osg::ref_ptr<osgFX::Scribe> cowScript= new osgFX::Scribe;
	cowScript->addChild(myCow);
	root->addChild(mt);
	root->addChild(myCow);
	root->addChild(cowScript);
	myViewer->setSceneData(root);
	myViewer->addEventHandler(new CPick(myViewer));
	myViewer->run();
}