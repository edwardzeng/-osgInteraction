#include "Pick.h"
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
		for (osgUtil::LineSegmentIntersector::Intersections::iterator it=lineSI.begin();it!=lineSI.end();it++)
		{
			if (!it->nodePath.empty()&&!(it->nodePath.back()->getName().empty()))
			{
				const osg::NodePath & np= it->nodePath;
				for (int i=0;i<np.size();i++)
				{
					osgFX::Scribe *sc=dynamic_cast<osgFX::Scribe *> (np[i]);
					if (sc!=NULL)
					{
						if(sc->getNodeMask()!=0)
						{
							sc->setNodeMask(0);
						}
						
					}
				}
			}
		}

	}
}
