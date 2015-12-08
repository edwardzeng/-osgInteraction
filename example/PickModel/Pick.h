
#ifndef PICK_H
#define PICK_H
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osgFX/Scribe>
#include <osgUtil/LineSegmentIntersector>
class CPick:public osgGA::GUIEventHandler
{
protected:
	osgViewer::Viewer * myViewer;
public:
	CPick(osgViewer::Viewer * viewer):myViewer(viewer){};
   virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
protected:
	void ToPick(float x,float y);
   
};


#endif