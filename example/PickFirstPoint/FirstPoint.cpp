/*OSG中的HUD实时显示视点坐标*/
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Geode>
#include <osg/Depth>
#include <osg/Camera>
#include <osgText/Text>
#include <osgGA/TrackballManipulator>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <iostream>
#include <sstream>
osg::ref_ptr<osg::Group> g_grpMouse;
using namespace std;
//事件类
class CHUD_viewPoint: public osgGA::GUIEventHandler  
{
public:  
	/**构造函数*/
	CHUD_viewPoint(osgText::Text* updateText):m_text(updateText) {}
	  ~CHUD_viewPoint(){}
	  virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa);
	  void UpdateText(osgViewer::Viewer* viewer,const osgGA::GUIEventAdapter&);
	  /**LABEL*/
	  void setLabel(const std::string& name)
	  {
		  if ( m_text.get())
		  {
			  m_text->setText(name); 
		  }
	  }
protected: 
	osg::Vec2 m_vPosWindowMouse;//鼠标单击处的窗口坐标
	osg::ref_ptr<osgText::Text>  m_text;//视点信息，会动态改变
};

bool CHUD_viewPoint::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
	switch(ea.getEventType())
	{
		//case(osgGA::GUIEventAdapter::FRAME):
		//  {
		//     osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		//     if (viewer)
		//     {
		//         osg::Vec3 vCenter, vUp;
		//         viewer->getCamera()->getViewMatrixAsLookAt( m_vPosEye, vCenter, vUp);//获取视点信息
		//         UpdateText( viewer, ea);//更新文字信息       
		//     }
		//     return true; 
		//  }    
	case( osgGA::GUIEventAdapter::PUSH):
		{
			m_vPosWindowMouse.set( ea.getX(), ea.getY());//鼠标单击处的窗口坐标
			osgViewer::Viewer* viewer = dynamic_cast< osgViewer::Viewer*>( &aa);
			if (viewer)
			{
				UpdateText( viewer, ea);//更新文字信息
				//主相机
				osg::ref_ptr<osg::Camera> cameraMaster = viewer->getCamera();
				osg::Matrix mvpw = cameraMaster->getViewMatrix() * cameraMaster->getProjectionMatrix();
				if ( cameraMaster->getViewport()) 
				{
					mvpw.postMult( cameraMaster->getViewport()->computeWindowMatrix());
				}
				osg::Matrix _inverseMVPW;
				_inverseMVPW.invert( mvpw);
				osg::Vec3d nearPoint = osg::Vec3d( ea.getX(), ea.getY(), 0.0)* _inverseMVPW;//透视投影中Znear平面的交点
				osg::Vec3d farPoint = osg::Vec3d( ea.getX(), ea.getY(), 1.0)* _inverseMVPW;//透视投影中Zfar平面的交点

				osg::Vec3 vPosEye, vCenter, vUp;
				cameraMaster->getViewMatrixAsLookAt( vPosEye, vCenter, vUp);//获取视点信息
				osg::Matrix _inverseMV;
				_inverseMV.invert( cameraMaster->getViewMatrix());
				osg::Vec3 ptEye= osg::Vec3(  0, 0, 0) * _inverseMV;//获取视点坐标
				osg::Vec3d deltaEye= ptEye- vPosEye;
				if ( deltaEye.length()< 1e-8)
				{
					cout<< "yes,eye\n";
				}
				else
				{
					cout<< "no,eye\n";
				}
				osg::Vec3d dir1= farPoint- nearPoint;
				dir1.normalize();
				osg::Vec3d dir2= farPoint- vPosEye;
				dir2.normalize();
				osg::Vec3d delta= dir1- dir2;
				//看视点、Znear平面的交点、Zfar平面的交点是否在同一直线上。经验证，确定在同一直线上
				if ( delta.length()< 1e-8)
				{
					cout<< "yes,line\n";
				}
				else
				{
					cout<< "no,line\n";
				}

				osg::Geode* geode= new osg::Geode();
				osg::Geometry* pyramidGeometry = new osg::Geometry();
				geode->addDrawable( pyramidGeometry);
				osg::Vec3Array* pyramidVertices = new osg::Vec3Array;

				osg::Vec3d eyePoint,farCenterPoint,farLeftUpPoint,farRightUpPoint,farLeftDownPoint,farRightDownPoint/*,lineNormal,windowPoint1,windowPoint2*/;
				/*eyePoint=vPosEye;*/
				//lineNormal=vCenter-vPosEye;
				//lineNormal.normalize();
				//farCenterPoint=vPosEye+lineNormal*30000;
				//windowPoint1=eyePoint*mvpw;
				//windowPoint2=farCenterPoint*mvpw;

				double fovy,aspectRatio,zNear,zFar;
				double farHalfWidth,farHalfHeight;
				cameraMaster->getProjectionMatrixAsPerspective(fovy,aspectRatio,zNear,zFar);
				farHalfHeight=zFar*tan(fovy/2*osg::PI/180);
				farHalfWidth=farHalfHeight*aspectRatio;

				eyePoint=osg::Vec3(0.0, 0.0, 0.0)* _inverseMV;
				farCenterPoint=osg::Vec3(0.0, 0.0, -zFar)* _inverseMV;
				farLeftUpPoint=osg::Vec3(-farHalfWidth, farHalfHeight,-zFar)* _inverseMV;
				farRightUpPoint=osg::Vec3(farHalfWidth, farHalfHeight, -zFar)* _inverseMV;
				farLeftDownPoint=osg::Vec3(-farHalfWidth, -farHalfHeight, -zFar)* _inverseMV;
				farRightDownPoint=osg::Vec3(farHalfWidth, -farHalfHeight, -zFar)* _inverseMV;

				pyramidVertices->push_back( eyePoint/*nearPoint*/);
				pyramidVertices->push_back( farCenterPoint/*farPoint*/);
				pyramidVertices->push_back( eyePoint);
				pyramidVertices->push_back( farLeftUpPoint);
				pyramidVertices->push_back( eyePoint);
				pyramidVertices->push_back( farRightUpPoint);
				pyramidVertices->push_back( eyePoint);
				pyramidVertices->push_back( farRightDownPoint);
				pyramidVertices->push_back( eyePoint);
				pyramidVertices->push_back( farLeftDownPoint);
				pyramidGeometry->setVertexArray( pyramidVertices );

				//颜色
				osg::Vec4Array* colors = new osg::Vec4Array;
				colors->push_back( osg::Vec4(  1.0f, 0.0f, 0.0f, 1.0f) );//红色
				colors->push_back( osg::Vec4(  0.0f, 1.0f, 0.0f, 1.0f) );
				colors->push_back( osg::Vec4(  0.0f, 0.0f, 1.0f, 1.0f) );
				colors->push_back( osg::Vec4(  1.0f, 1.0f, 1.0f, 1.0f) );
				colors->push_back( osg::Vec4(  0.0f, 0.0f, 0.0f, 1.0f) );
				pyramidGeometry->setColorArray( colors);
				pyramidGeometry->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE_SET);


				//红线表示鼠标点击的线，其起点为Znear平面交点，终点为Zfar平面交点。
				for (int i=0;i<5;++i)
				{
					pyramidGeometry->addPrimitiveSet(  new osg::DrawArrays( osg::PrimitiveSet::LINES, 2*i, 2));/**/
				}
				//红点表示透视投影中Znear平面的交点
				pyramidGeometry->addPrimitiveSet(  new osg::DrawArrays( osg::PrimitiveSet::POINTS, 0, 1/*3*/));


				//设置线宽
				osg::ref_ptr <osg::LineWidth> LineSize = new osg::LineWidth;
				LineSize ->setWidth( 5.0) ;      
				geode->getOrCreateStateSet()->setAttributeAndModes( LineSize.get (),osg::StateAttribute::ON);
				//设置点大小
				osg::ref_ptr <osg::Point> ptSize = new osg::Point;
				ptSize->setSize( 20.0) ;      
				geode->getOrCreateStateSet()->setAttributeAndModes( ptSize.get(),osg::StateAttribute::ON);


				/*当只有一个点时，包围球半径为，所以可能看不到这个点，故需要重新设置包围球大小，可把包围球半径设大点。
				如对glider、cow等小模型，半径取.1可以，对fountain.osg则.1太小。为统一，可大些，如*/
				osg::Vec3d ptCnt= geode->getBound().center();
				double dRadius= geode->getBound().radius();
				//重新设置包围球的半径（可调用setInitialBound()）
				osg::BoundingSphere bs( ptCnt, 100);
				geode->setInitialBound( bs);

				g_grpMouse->removeChildren( 0, g_grpMouse->getNumChildren());
				g_grpMouse->addChild( geode);

				//获取从根节点到当前节点的路径向量
				osg::NodePathList parentNodePaths = geode->getParentalNodePaths();
				if ( !parentNodePaths.empty())
				{
					osg::Matrixd mt= computeWorldToLocal( parentNodePaths[ 0]);
				}


				//方法一：求交
				//osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(
				//  osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
				//osgUtil::IntersectionVisitor iv( picker.get());
				//cameraMaster->accept( iv);//模型求交(从相机往下遍历)

				//方法二：求交
				osg::ref_ptr< osgUtil::LineSegmentIntersector > picker =new osgUtil::LineSegmentIntersector(eyePoint,farCenterPoint/*nearPoint, farPoint*/);//线段(真实的世界坐标)
				osgUtil::IntersectionVisitor iv( picker.get());
				//g_grpMouse->getParent( 0)->getChild( 0)->asGroup()->getChild( 0)->accept( iv);//模型求交/**/
				g_grpMouse->getParent( 0)->getChild( 0)->accept( iv);//模型求交/**/

				////方法三：求交
				//osgUtil::LineSegmentIntersector::Intersections intersections;
				//bool isContainsIntersections=viewer->computeIntersections( windowPoint1.x(),windowPoint1.y(),/*ea.getX(), ea.getY(),*/ intersections);

				// 最前交点，方法一：              根节点     cow的MT节点
				//cameraMaster->accept( iv);//模型求交(从相机往下遍历)
				//if (picker->containsIntersections())
				//{
				//  double dLen2Shortest= DBL_MAX, dLenTmp;
				//  osgUtil::LineSegmentIntersector::Intersections::iterator hitrShortest;
				//  osgUtil::LineSegmentIntersector::Intersections intersections= picker->getIntersections();
				//  for( osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
				//     hitr != intersections.end();
				//     ++hitr)
				//  {
				//     //求离视点最近的点，即鼠标选择的最前面的点
				//     dLenTmp= ( ptEye.x()- hitr->getWorldIntersectPoint().x())*
				//         ( ptEye.x()- hitr->getWorldIntersectPoint().x())+
				//         ( ptEye.y()- hitr->getWorldIntersectPoint().y())*
				//         ( ptEye.y()- hitr->getWorldIntersectPoint().y())+
				//         ( ptEye.z()- hitr->getWorldIntersectPoint().z())*
				//         ( ptEye.z()- hitr->getWorldIntersectPoint().z());
				//     if ( dLen2Shortest> dLenTmp)
				//     {
				//         dLen2Shortest= dLenTmp;
				//         hitrShortest= hitr;                   
				//     }
				//  }
				// 
				//  //输出
				//  if ( dLen2Shortest != DBL_MAX)
				//  {
				//     cout<<"world coords vertex("<< hitrShortest->getWorldIntersectPoint().x()<<","
				//         << hitrShortest->getWorldIntersectPoint().y()<<","
				//         << hitrShortest->getWorldIntersectPoint().z()<<")"<<std::endl;
				//     //高亮此点              
				//     double dPointRadius= 15.0f;
				//     osg::ShapeDrawable* pShd= new osg::ShapeDrawable(
				//         new osg::Sphere( hitrShortest->getWorldIntersectPoint(), dPointRadius));//绘制交点的球
				//     pShd->setColor( osg::Vec4( 0, 1, 0, 1));
				//     geode->addDrawable( pShd);
				//    
				//  }
				//}

				////求最前交点，方法二：如果包含交叉点，则在第一个交点处显示一个蓝色的球
				//if(isContainsIntersections)
				//{
				//	osg::Vec3 ptWorldIntersectPointFirst;
				//	osgUtil::LineSegmentIntersector::Intersections::iterator iter=intersections.begin();
				//	ptWorldIntersectPointFirst.set(iter->getWorldIntersectPoint().x(),iter->getWorldIntersectPoint().y(),iter->getWorldIntersectPoint().z());
				//	//osg::Vec3 ptWorldIntersectPointFirst= picker->getFirstIntersection().getWorldIntersectPoint();
				//	cout<<"world coords vertex("<< ptWorldIntersectPointFirst.x()<<","
				//		<< ptWorldIntersectPointFirst.y()<< ","<< ptWorldIntersectPointFirst.z()<<")"<< std::endl;
				//	//高亮此点              
				//	double dPointRadius= 150.0f;
				//	osg::ShapeDrawable* pShd= new osg::ShapeDrawable(new osg::Sphere( ptWorldIntersectPointFirst, dPointRadius));
				//	pShd->setColor( osg::Vec4( 0, 0, 1, 1));
				//	geode->addDrawable(pShd);
				//}
				//求最前交点，方法三：如果包含交叉点，则在第一个交点处显示一个蓝色的球
				if (picker->containsIntersections())
				{
					osg::Vec3 ptWorldIntersectPointFirst= picker->getFirstIntersection().getWorldIntersectPoint();
					cout<<"world coords vertex("<< ptWorldIntersectPointFirst.x()<<","
						<< ptWorldIntersectPointFirst.y()<< ","<< ptWorldIntersectPointFirst.z()<<")"<< std::endl;
					//高亮此点              
					double dPointRadius= 150.0f;
					osg::ShapeDrawable* pShd= new osg::ShapeDrawable(new osg::Sphere( ptWorldIntersectPointFirst, dPointRadius));
					pShd->setColor( osg::Vec4( 0, 0, 1, 1));
					geode->addDrawable( pShd);
				}
			}
			return true;
		}
	default:     
		return false; 
	}
}
void CHUD_viewPoint::UpdateText(osgViewer::Viewer* viewer,const osgGA::GUIEventAdapter&)
{
	std::string gdlist=""; 
	std::ostringstream os;
	os<<"MousePos(X: "<< m_vPosWindowMouse.x()<<",Y: "<< m_vPosWindowMouse.y()<<")";//坐标
	gdlist = os.str(); 
	setLabel(gdlist); 
}
osg::Node* createHUD_viewPoint( osgText::Text* text)
{
	//设置字体
	std::string font("fonts/arial.TTF");//此处设置的是汉字字体 "fonts/STCAIYUN.TTF"
	text->setFont( font); 
	//设置文字显示的位置(左下为(0,0),X正向朝右，Y正向朝上)
	osg::Vec3 position( 100.0f, 10.0f,0.0f);
	text->setPosition(position);   
	text->setColor( osg::Vec4( 1, 1, 0, 1));
	text->setText(L"");//设置显示的文字
	text->setCharacterSize(15);
	text->setDataVariance(osg::Object::DYNAMIC);//一定要设置字体为动态，否则程序会卡住，死在那里。（参照osgcatch）
	//几何体节点
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable( text );//将文字Text作这drawable加入到Geode节点中
	//设置状态
	osg::StateSet* stateset = geode->getOrCreateStateSet(); 
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);//关闭灯光
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);//关闭深度测试
	//打开GL_BLEND混合模式（以保证Alpha纹理正确）
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	//相机
	osg::Camera* camera = new osg::Camera;
	//设置透视矩阵
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0,600,0,600));//正交投影   
	//设置绝对参考坐标系，确保视图矩阵不会被上级节点的变换矩阵影响
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	//视图矩阵为默认的
	camera->setViewMatrix(osg::Matrix::identity());
	//设置背景为透明，否则的话可以设置ClearColor 
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setAllowEventFocus( false);//不响应事件，始终得不到焦点
	//设置渲染顺序，必须在最后渲染
	camera->setRenderOrder(osg::Camera::POST_RENDER); 
	camera->addChild(geode);//将要显示的Geode节点加入到相机
	return camera; 
};

void createGraphicsWindow( osgViewer::Viewer& viewer,int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
	//GraphicsContext设备上下文关键参数
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits ;
	traits->windowName = name;
	traits->x = x; //屏幕左上角为坐标原点，横轴为x，竖轴为y（x，y点是界面开始点）
	traits->y = y; 
	traits->width = w;//横轴
	traits->height = h; //竖轴
	traits->windowDecoration = windowDecoration; 
	traits->doubleBuffer = true;
	traits->sharedContext = 0; 
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	gc->setClearColor( osg::Vec4f( 1.0f, 0.0f, 0.0f, 1.0f)); //设置整个windows窗口颜色
	gc->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//主相机
	osg::ref_ptr<osg::Camera> cameraMaster = viewer.getCamera();
	cameraMaster->setGraphicsContext(gc.get());//设置GraphicsContext设备上下文
	//相机视口设置
	cameraMaster->setViewport(new osg::Viewport( 50, 50, traits->width, traits->height));/*左下角为坐标原点，横轴为x，竖轴为y*/
}

int main( int argc, char **argv )
{
	osgViewer::Viewer viewer; 
	osg::ref_ptr<osg::Group> root= new osg::Group; 

	//osg::ref_ptr< osg::Node> model = osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/fountain.osgt");// glider nathan
	osg::ref_ptr< osg::Node> model = osgDB::readNodeFile("E:/SourceCode/OSG/OSGData/Robot/right/car.ive");
	osg::ref_ptr< osg::MatrixTransform> pmt= new osg::MatrixTransform();
	pmt->addChild( model.get());
	root->addChild( pmt.get());//加入某个模型
	osg::ref_ptr<osg::Camera> cameraSlave;
	pmt->addChild(cameraSlave);

	osgText::Text* text = new osgText::Text; 
	root->addChild( createHUD_viewPoint( text));//加入HUD文字
	osg::ref_ptr< CHUD_viewPoint> pHUD= new CHUD_viewPoint( text);
	viewer.addEventHandler( pHUD.get());

	std::string name="first point";
	createGraphicsWindow(viewer,300,33,800,600,name,true);
	g_grpMouse= new osg::Group();
	//设置状态
	osg::StateSet* stateset = g_grpMouse->getOrCreateStateSet(); 
	stateset->setMode( GL_LIGHTING,osg::StateAttribute::OFF);//关闭灯光
	stateset->setMode( GL_DEPTH_TEST,osg::StateAttribute::ON);//打开深度测试，否则指示的交点处的球会无法遮挡，总是看见，浮现
	root->addChild( g_grpMouse.get());

	//viewer.setUpViewInWindow( 100, 50, 600, 600);//设置窗口大小，实现与createGraphicsWindow类似的效果
	viewer.setSceneData( root.get());
	viewer.realize();
	viewer.run() ;  
	return 0;
}

///**********************************************************
//*Write by FlySky
//*zzuxp@163.com  http://www.OsgChina.org   
//**********************************************************/
//
//#include <osgViewer/Viewer>
//#include <osgViewer/ViewerEventHandlers>
//
//#include <osg/Node>
//#include <osg/Geode>
//#include <osg/Group>
//#include <osg/Geometry>
//#include <osg/MatrixTransform>
//#include <osg/PositionAttitudeTransform>
//#include <osg/Camera>
//#include <osg/MatrixTransform>
//
//#include <osgDB/ReadFile>
//#include <osgDB/WriteFile>
//
//#include <osgUtil//Optimizer>
//
//#include <iostream>
//
////单窗口多相机渲染
//osg::Camera* cameraClient = new osg::Camera();
//void singleWindowMultipleCameras(osgViewer::Viewer* viewer)
//{
//	//创建窗口系统接口
//	osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
//	if (!wsi) 
//	{
//		osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
//		return;
//	}
//
//	//得到窗口分辨率
//	unsigned int width, height;
//	wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
//
//	//设置图形环境特性
//	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
//	traits->x = 0;
//	traits->y = 0;
//	traits->width = width;
//	traits->height = height;
//	traits->windowDecoration = true;
//	traits->doubleBuffer = true;
//	traits->sharedContext = 0;
//
//	//创建图形环境
//	osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());
//
//	if (gc->valid())
//	{
//		osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;
//
//		//确保窗口清除干净
//		gc->setClearColor(osg::Vec4f(0.2f,0.2f,0.6f,1.0f));
//		gc->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	}
//	else
//	{
//		osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
//	}
//
//	//得到cameraMaster(主相机)
//	osg::Camera* cameraMaster = viewer->getCamera();
//	//设置图形环境
//	cameraMaster->setGraphicsContext(gc);
//
//	//根据分辨率来确定合适的投影来保证显示的图形不变形
//	double fovy, aspectRatio, zNear, zFar;
//	cameraMaster->getProjectionMatrixAsPerspective(fovy, aspectRatio, zNear, zFar);
//	double newAspectRatio = double(traits->width) / double(traits->height);
//	double aspectRatioChange = newAspectRatio / aspectRatio;
//	if (aspectRatioChange != 1.0)
//	{
//		cameraMaster->getProjectionMatrix() *= osg::Matrix::scale(1.0/aspectRatioChange,1.0,1.0);
//	}
//
//	//设置视口
//	cameraMaster->setViewport(new osg::Viewport(0,0, width, height));
//	GLenum bufferMaster = traits->doubleBuffer ? GL_BACK : GL_FRONT;
//	//设置缓冲区
//	cameraMaster->setDrawBuffer(bufferMaster);
//	cameraMaster->setReadBuffer(bufferMaster);
//
//	//创建从属相机
//	
//	cameraClient->setGraphicsContext(gc);
//	cameraClient->setViewport(new osg::Viewport(0,0, 400, 400));
//	GLenum bufferClient = traits->doubleBuffer ? GL_BACK : GL_FRONT;
//	cameraClient->setDrawBuffer(bufferClient);
//	cameraClient->setReadBuffer(bufferClient);
//
//	//添加从属相机
//	viewer->addSlave(cameraClient,osg::Matrix::scale(aspectRatio,1.0,1.0),osg::Matrix()) ;
//
//}
//
//int main()
//{
//	osgViewer::Viewer* viewer =new osgViewer::Viewer();
//
//	osg::Node* cow = new osg::Node() ;
//	cow = osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/cow.osg");
//	osg::ref_ptr<osg::MatrixTransform> transModel=new osg::MatrixTransform;
//	transModel->addChild(cow);
//	osg::ref_ptr<osg::MatrixTransform> transCamera=new osg::MatrixTransform;
//	transCamera->setMatrix(osg::Matrix::translate(0.0,100,0.0));
//	transModel->addChild(transCamera);
//	//transCamera->addChild(cameraClient);
//
//	//启用单视图多相机渲染
//	singleWindowMultipleCameras(viewer);
//
//	//优化场景数据
//	osgUtil::Optimizer optimizer ;
//	optimizer.optimize(transModel);
//
//	viewer->setSceneData(transModel);
//
//	viewer->realize();
//
//	viewer->run();
//
//	return 0;
//}