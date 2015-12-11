#include "Pick.h"
#include <iostream>
bool CPick::handle( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa )
{
	if (myViewer)
	{
		switch(ea.getEventType())
		{
		case osgGA::GUIEventAdapter::PUCK:
			{
				if (ea.getButton()==1)
				{
					ToPick(ea.getX(),ea.getY());
				}

			}
			return true;
		}
	}
	return false;

}

void CPick::ToPick(float x,float y)
{
	
	
	osgUtil::LineSegmentIntersector::Intersections lineSI;
	if(myViewer->computeIntersections(x,y,lineSI))
	{
		osgUtil::LineSegmentIntersector::Intersections::iterator it=lineSI.begin();
		//for (osgUtil::LineSegmentIntersector::Intersections::iterator it=lineSI.begin();it!=lineSI.end();it++)
		//{
			if (!it->nodePath.empty()&&!(it->nodePath.back()->getName().empty()))
			{
				const osg::NodePath & np= it->nodePath;
				for (int i=0;i<np.size();i++)
				{
					osg::Node* nd=dynamic_cast<osg::Node*>(np[i]);
					osgFX::Scribe *sc=dynamic_cast<osgFX::Scribe *> (np[i]);
					if (sc!=NULL)
					{
						if(sc->getNodeMask()!=0)
						{
							sc->setNodeMask(0);
						}
						
					}
					if (nd->getName()=="cow.osg")
					{
						std::cout<<"found cow"<<std::endl;
					}
					std::cout<<"not found cow"<<std::endl;
				}
				std::cout<<"-------------------- "<<std::endl<<std::endl;
			}
		//}  

	}
}
