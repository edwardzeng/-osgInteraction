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
//对象选取事件处理器
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
	  //事件处理函数
	  bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
	  {
		  osg::ref_ptr<osgViewer::View> view = dynamic_cast<osgViewer::View*>(&aa);
		  if (!view) return false;
		  switch(ea.getEventType())
		  {
			  //鼠标按下
		  case(osgGA::GUIEventAdapter::PUSH):
			  {
				  //更新鼠标位置
				  _mx = ea.getX();
				  _my = ea.getY();
				  pick(view.get(), ea.getX(), ea.getY());
				  break;
			  }
		  case(osgGA::GUIEventAdapter::RELEASE):
			  {
				  if (_mx==ea.getX() && _my==ea.getY())
				  {
					  //执行对象选取
					  //pick(view.get(), ea.getX(), ea.getY());
				  }
				  break;
			  }
		  default:
			  break;
		  }
		  return false;
	  }
	  //对象选取事件处理器
	  void pick(osg::ref_ptr<osgViewer::View> view, float x, float y)
	  {
		  osg::ref_ptr<osg::Node> node;
		  osg::ref_ptr<osg::Group> parent;
		  //创建一个线段交集检测函数
		  osgUtil::LineSegmentIntersector::Intersections intersections;
		  if (view->computeIntersections(x, y, intersections))
		  {
			  osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
			  osg::NodePath& nodePath = intersection.nodePath;
			  //得到选择的物体
			  node = (nodePath.size()>=1)?nodePath[nodePath.size()-1]:0;
			  parent = (nodePath.size()>=2)?dynamic_cast<osg::Group*>(nodePath[nodePath.size()-2]):0;
		  }       
		  //用一种高亮显示来显示物体已经被选中
		  if (parent.get() && node.get())
		  {
			  bool isSelectSameNode=false;
			  if (_lastSelectNode)
			  {
				  if (_lastSelectNode && _lastSelectNode !=node)
				  {
					  //如果选择其他物体，则移除高亮显示的对象
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
				  //如果对象选择到，高亮显示
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
	//得到鼠标的位置
	float _mx ;
	float _my;
	osg::Node* _lastSelectNode;
	osg::Group* _lastSelectParent;
	osgFX::Scribe* _lastScribe;
};
int main()
{
	//创建Viewer对象，场景浏览器
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	viewer->addEventHandler(new PickHandler());
	//创建场景组节点
	osg::ref_ptr<osg::Group> root = new osg::Group();
	//创建一个节点，读取牛的模型
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cow.osg");
	osg::ref_ptr<osg::Node> myCessna= osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cessna.osg");

	osg::ref_ptr<osg::Node> car= osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/dumptruck.osg");
	//添加到场景
	root->addChild(node.get());
	root->addChild(myCessna);
	root->addChild(car);
	//优化场景数据
	osgUtil::Optimizer optimizer ;
	optimizer.optimize(root.get()) ;
	viewer->setSceneData(root.get());
	viewer->setUpViewInWindow(300,50,800,600);
	viewer->realize();
	viewer->run();
	return 0 ;
}