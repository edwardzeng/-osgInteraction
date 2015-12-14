#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/MatrixTransform>
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
	  void PickHandler::pick(osg::ref_ptr<osgViewer::View> view, float x, float y)
	  {
		  osg::ref_ptr<osg::Node> clickedNode;
		  osg::ref_ptr<osg::Node> node3;
		  osg::ref_ptr<osg::Group> clickedNodeParent;
		  std::string clickedNodeName;

		  //����һ���߶ν�����⺯��
		  osgUtil::LineSegmentIntersector::Intersections intersections;
		  if (view->computeIntersections(x, y, intersections))
		  {
			  osgUtil::LineSegmentIntersector::Intersections::iterator it = intersections.begin();
			  const osg::NodePath& nodePath = it->nodePath;

			  if ( !it->nodePath.empty() && !(it->nodePath.back()->getName().empty()) && nodePath.size()>=3)
			  {
				  //const osg::NodePath & nodePath= it->nodePath;
				  for (int i=0;i<nodePath.size();i++)
				  {
					  osg::Node* nd=dynamic_cast<osg::Node*>(nodePath[i]);
					  std::string nodeName=nd->getName();
					  std::string name;
					  int length=nodeName.size();
					  if (length>=4)
					  {
						  for (int j=0;j<4;++j)
						  {
							  name +=nodeName[length-4+j];
						  }

						  if (name==".ive")
						  {
							  clickedNodeName=nodeName;
							  std::cout<<"get the clicked node: "<<clickedNodeName<<std::endl;
							  clickedNode =nd;
							  clickedNodeParent = dynamic_cast<osg::Group*>(nodePath[i-1]);
						  }
					  }
					  std::cout<<"node"<<i<<":"<<nd->getName()<<std::endl;
				  }
				  std::cout<<"-------------------- "<<std::endl<<std::endl;
			  }
			  else
			  {
				  clickedNode=0;
				  clickedNodeParent=0;
			  }
		  }       
		  //��һ�ָ�����ʾ����ʾ�����Ѿ���ѡ��
		  if (clickedNodeParent.get() && clickedNode.get())
		  {
			  bool isSelectSameNode=false;
			  if (_lastSelectNode)
			  {
				  if (_lastSelectNodeName !=clickedNodeName)
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
				  _lastSelectNode=clickedNode;
				  _lastSelectParent=clickedNodeParent;
				  _lastSelectNodeName=clickedNode->getName();

				  osg::ref_ptr<osgFX::Scribe> scribe = new osgFX::Scribe();
				  scribe->addChild(clickedNode.get());
				  std::string scribeName="scribe "+clickedNode->getName();
				  scribe->setName(scribeName);
				  _lastScribe=scribe;
				  clickedNodeParent->replaceChild(clickedNode.get(),scribe.get());
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
	std::string _lastSelectNodeName;
};
int main()
{
	//����Viewer���󣬳��������
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	viewer->addEventHandler(new PickHandler());
	//����������ڵ�
	osg::ref_ptr<osg::Group> root = new osg::Group();
	osg::ref_ptr<osg::MatrixTransform> transform1=new osg::MatrixTransform;
	osg::ref_ptr<osg::MatrixTransform> transform2=new osg::MatrixTransform;

	osg::ref_ptr<osg::Node> arm1 = osgDB::readNodeFile("E:/SourceCode/OSG/OSGData/Robot/right/arm1.ive");
	osg::ref_ptr<osg::Node> cow= osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cow.osg");
	osg::ref_ptr<osg::Node> car= osgDB::readNodeFile("E:/SourceCode/OSG/OSGData/Robot/car.osg");

	arm1->setName("arm1.ive");
	cow->setName("cow.osg");
	car->setName("car.ive");
	viewer->getCamera()->setName("master camera");

	transform1->setName("tran1");
	transform2->setName("tran2");
	transform1->addChild(arm1);

	transform1->addChild(transform2);
	transform2->setMatrix(osg::Matrix::translate(0.,0.,2000.));
	transform2->addChild(car);
	root->setName("root group");
	//��ӵ�����
	//root->addChild(node);
	//root->addChild(cow);
	//root->addChild(car);
	root->addChild(transform1);
	//�Ż���������
	osgUtil::Optimizer optimizer ;
	optimizer.optimize(root.get()) ;
	viewer->setSceneData(root.get());
	viewer->setUpViewInWindow(300,50,800,600);
	viewer->realize();
	viewer->run();
	return 0 ;
}