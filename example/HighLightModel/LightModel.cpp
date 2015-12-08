#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgFX/Scribe>
#include <osgGA/GUIEventHandler>
#include <osgUtil/Optimizer>
#include <iostream>
//����ѡȡ�¼�������
class PickHandler : public osgGA::GUIEventHandler
{
public:
	PickHandler():
	  _mx(0.0f),
		  _my(0.0f),
		  _lastSelectNode(0)
	  {
		  //
	  }
	  ~PickHandler()
	  {
		  //
	  }
	  //�¼�������
	  bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
	  {
		  osg::ref_ptr<osgViewer::View> view = dynamic_cast<osgViewer::View*>(&aa);
		  if (!view) return false;
		  switch(ea.getEventType())
		  {
			  //��갴��
		  case(osgGA::GUIEventAdapter::PUSH):
			  {
				  //�������λ��
				  _mx = ea.getX();
				  _my = ea.getY();
				  pick(view.get(), ea.getX(), ea.getY());
				  break;
			  }
		  case(osgGA::GUIEventAdapter::RELEASE):
			  {
				  if (_mx==ea.getX() && _my==ea.getY())
				  {
					  //ִ�ж���ѡȡ
					  //pick(view.get(), ea.getX(), ea.getY());
				  }
				  break;
			  }
		  default:
			  break;
		  }
		  return false;
	  }
	  //����ѡȡ�¼�������
	  void pick(osg::ref_ptr<osgViewer::View> view, float x, float y)
	  {
		  osg::ref_ptr<osg::Node> node;
		  osg::ref_ptr<osg::Group> parent;
		  //����һ���߶ν�����⺯��
		  osgUtil::LineSegmentIntersector::Intersections intersections;
		  if (view->computeIntersections(x, y, intersections))
		  {
			  osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
			  osg::NodePath& nodePath = intersection.nodePath;
			  //�õ�ѡ�������
			  node = (nodePath.size()>=1)?nodePath[nodePath.size()-1]:0;
			  parent = (nodePath.size()>=2)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-2]):0;
		  }       
		  //��һ�ָ�����ʾ����ʾ�����Ѿ���ѡ��
		  if (parent.get() && node.get())
		  {
			  bool isSelectSameNode=false;
			  if (_lastSelectNode)
			  {
				  if (_lastSelectNode && _lastSelectNode !=node)
				  {
					  //���ѡ���������壬���Ƴ�������ʾ�Ķ���
					  _lastSelectParent->replaceChild(_lastScribe,_lastSelectNode);
					  _lastSelectNode=0;
				  }
				  else
				  {
					  isSelectSameNode=true;
				  }
			  }
			  if (!isSelectSameNode)
			  {
				  //�������ѡ�񵽣�������ʾ
				  _lastSelectNode=node;
				  _lastSelectParent=parent;
				  osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe();
				  scribe->addChild(node.get());
				  _lastScribe=scribe;
				  parent->replaceChild(node.get(),scribe.get());
			  }
		  }
	  }
public:
	//�õ�����λ��
	float _mx ;
	float _my;
	osg::Node* _lastSelectNode;
	osg::Group* _lastSelectParent;
	osgFX::Scribe* _lastScribe;
};
int main()
{
	//����Viewer���󣬳��������
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	viewer->addEventHandler(new PickHandler());
	//����������ڵ�
	osg::ref_ptr<osg::Group> root = new osg::Group();
	//����һ���ڵ㣬��ȡţ��ģ��
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cow.osg");
	osg::ref_ptr<osg::Node> myCessna= osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cessna.osg");

	osg::ref_ptr<osg::Node> car= osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/dumptruck.osg");
	//��ӵ�����
	root->addChild(node.get());
	root->addChild(myCessna);
	root->addChild(car);
	//�Ż���������
	osgUtil::Optimizer optimizer ;
	optimizer.optimize(root.get()) ;
	viewer->setSceneData(root.get());
	viewer->setUpViewInWindow(300,50,800,600);
	viewer->realize();
	viewer->run();
	return 0 ;
}