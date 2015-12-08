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
	CHUD_viewPoint(osgText::Text* updateText):
	  m_text(updateText) {}
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
           if ( cameraMaster->getViewport()) mvpw.postMult( cameraMaster->getViewport()->computeWindowMatrix());
 
           osg::Matrix _inverseMVPW;
           _inverseMVPW.invert( mvpw);
 
           osg::Vec3d nearPoint = osg::Vec3d( ea.getX(), ea.getY(), 0.0)* _inverseMVPW;//透视投影中Znear平面的交点
           osg::Vec3d farPoint = osg::Vec3d( ea.getX(), ea.getY(), 1.0)* _inverseMVPW;//透视投影中Zfar平面的交点
 
           ////求交
           //osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(
           // osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
           //osgUtil::IntersectionVisitor iv( picker.get());
           ////g_grpMouse->getParent( 0)->getChild( 0)->accept( iv);//模型求交
           //cameraMaster->accept( iv);//模型求交(从相机往下遍历)
 
           //求交
           osg::ref_ptr< osgUtil::LineSegmentIntersector > picker =new osgUtil::LineSegmentIntersector(nearPoint, farPoint);//线段(真实的世界坐标)
           osgUtil::IntersectionVisitor iv( picker.get());
           //g_grpMouse->getParent( 0)->getChild( 0)->accept( iv);//模型求交/**/
           g_grpMouse->getParent( 0)->getChild( 0)->/*asGroup()->getChild( 0)->*/accept( iv);//模型求交/**/
            //               根节点     cow的MT节点
           //cameraMaster->accept( iv);//模型求交(从相机往下遍历)
 
          
           if (picker->containsIntersections())
           {
              osg::Vec3 ptWorldIntersectPointFirst= picker->getFirstIntersection().getWorldIntersectPoint();
              cout<<"world coords vertex("<< ptWorldIntersectPointFirst.x()<<","
                  << ptWorldIntersectPointFirst.y()<< ","<< ptWorldIntersectPointFirst.z()<<")"<< std::endl;
              //高亮此点              
              double dPointRadius= 15.0f;
              osg::ShapeDrawable* pShd= new osg::ShapeDrawable(
                  new osg::Sphere( ptWorldIntersectPointFirst, dPointRadius));
              pShd->setColor( osg::Vec4( 0, 1, 0, 1));
              //geode->addDrawable( pShd);

              ////将此Drawable高亮显示(更改其材质－－散射光)             
             //osg::ref_ptr< osg::StateSet> pssDra= picker->getFirstIntersection().drawable->getOrCreateStateSet();
              ////改变颜色
              //osg::Material* mat = dynamic_cast< osg::Material*>
             // ( pssDra->getAttribute( osg::StateAttribute::MATERIAL));             
             //if ( mat == NULL)
              //{
              /// mat = new osg::Material;
              // mat->setDataVariance( osg::Object::DYNAMIC);
              // pssDra->setAttribute( mat);
              //}
              //mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( 1, 0, 0, 0));
 
              //直接改变其颜色
              osg::ref_ptr< osg::Geometry> pGmt= picker->getFirstIntersection().drawable->asGeometry();
              if ( pGmt != NULL)
              {                
                  //颜色(先获取原始颜色数组，再调用dirty()更新)
                  osg::Vec4Array* pv4aColorsOld=  dynamic_cast< osg::Vec4Array*> ( pGmt->getColorArray());
                  if ( pv4aColorsOld != NULL)
                  {
                     pv4aColorsOld->clear();
                     pv4aColorsOld->push_back( osg::Vec4(  1.0f, 0.0f, 0.0f, 1.0f) );//红色
 
                     pv4aColorsOld->dirty();//更新颜色数组
                      pGmt->getOrCreateStateSet()->setMode( GL_LIGHTING,osg::StateAttribute::OFF|
                         osg::StateAttribute::OVERRIDE);//关闭灯光
 
                     }
                  else
                  {
                     pv4aColorsOld= new osg::Vec4Array;
                     pv4aColorsOld->push_back( osg::Vec4(  1.0f, 0.0f, 0.0f, 1.0f) );//红色
                     pGmt->setColorArray( pv4aColorsOld);
                      pGmt->setColorBinding( osg::Geometry::BIND_OVERALL/*BIND_PER_PRIMITIVE_SET:*/);
 
                      pGmt->getOrCreateStateSet()->setMode( GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);//关闭灯光
                  }
              }
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
	osg::ref_ptr< osg::Node> model = osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/fountain.osgt");// glider nathan
	osg::ref_ptr< osg::MatrixTransform> pmt= new osg::MatrixTransform();
	pmt->addChild( model.get());
	root->addChild( pmt.get());//加入某个模型
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
	stateset->setMode( GL_DEPTH_TEST,osg::StateAttribute::OFF);//关闭深度测试
	root->addChild( g_grpMouse.get());
	//viewer.setUpViewInWindow( 100, 50, 600, 600);//设置窗口大小，实现与createGraphicsWindow类似的效果
	viewer.setSceneData( root.get());
	viewer.realize();
	viewer.run() ;  
	return 0;
}