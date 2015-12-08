/*OSG�е�HUDʵʱ��ʾ�ӵ�����*/
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
//�¼���
class CHUD_viewPoint: public osgGA::GUIEventHandler  
{
public:  
	/**���캯��*/
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
	osg::Vec2 m_vPosWindowMouse;//��굥�����Ĵ�������
	osg::ref_ptr<osgText::Text>  m_text;//�ӵ���Ϣ���ᶯ̬�ı�
};
bool CHUD_viewPoint::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa)
{
    switch(ea.getEventType())
    {
    case( osgGA::GUIEventAdapter::PUSH):
    {
       m_vPosWindowMouse.set( ea.getX(), ea.getY());//��굥�����Ĵ�������
      
       osgViewer::Viewer* viewer = dynamic_cast< osgViewer::Viewer*>( &aa);
      
       if (viewer)
       {
           UpdateText( viewer, ea);//����������Ϣ
 
           //�����
           osg::ref_ptr<osg::Camera> cameraMaster = viewer->getCamera();
 
           osg::Matrix mvpw = cameraMaster->getViewMatrix() * cameraMaster->getProjectionMatrix();
           if ( cameraMaster->getViewport()) mvpw.postMult( cameraMaster->getViewport()->computeWindowMatrix());
 
           osg::Matrix _inverseMVPW;
           _inverseMVPW.invert( mvpw);
 
           osg::Vec3d nearPoint = osg::Vec3d( ea.getX(), ea.getY(), 0.0)* _inverseMVPW;//͸��ͶӰ��Znearƽ��Ľ���
           osg::Vec3d farPoint = osg::Vec3d( ea.getX(), ea.getY(), 1.0)* _inverseMVPW;//͸��ͶӰ��Zfarƽ��Ľ���
 
           ////��
           //osg::ref_ptr< osgUtil::LineSegmentIntersector > picker = new osgUtil::LineSegmentIntersector(
           // osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
           //osgUtil::IntersectionVisitor iv( picker.get());
           ////g_grpMouse->getParent( 0)->getChild( 0)->accept( iv);//ģ����
           //cameraMaster->accept( iv);//ģ����(��������±���)
 
           //��
           osg::ref_ptr< osgUtil::LineSegmentIntersector > picker =new osgUtil::LineSegmentIntersector(nearPoint, farPoint);//�߶�(��ʵ����������)
           osgUtil::IntersectionVisitor iv( picker.get());
           //g_grpMouse->getParent( 0)->getChild( 0)->accept( iv);//ģ����/**/
           g_grpMouse->getParent( 0)->getChild( 0)->/*asGroup()->getChild( 0)->*/accept( iv);//ģ����/**/
            //               ���ڵ�     cow��MT�ڵ�
           //cameraMaster->accept( iv);//ģ����(��������±���)
 
          
           if (picker->containsIntersections())
           {
              osg::Vec3 ptWorldIntersectPointFirst= picker->getFirstIntersection().getWorldIntersectPoint();
              cout<<"world coords vertex("<< ptWorldIntersectPointFirst.x()<<","
                  << ptWorldIntersectPointFirst.y()<< ","<< ptWorldIntersectPointFirst.z()<<")"<< std::endl;
              //�����˵�              
              double dPointRadius= 15.0f;
              osg::ShapeDrawable* pShd= new osg::ShapeDrawable(
                  new osg::Sphere( ptWorldIntersectPointFirst, dPointRadius));
              pShd->setColor( osg::Vec4( 0, 1, 0, 1));
              //geode->addDrawable( pShd);

              ////����Drawable������ʾ(��������ʣ���ɢ���)             
             //osg::ref_ptr< osg::StateSet> pssDra= picker->getFirstIntersection().drawable->getOrCreateStateSet();
              ////�ı���ɫ
              //osg::Material* mat = dynamic_cast< osg::Material*>
             // ( pssDra->getAttribute( osg::StateAttribute::MATERIAL));             
             //if ( mat == NULL)
              //{
              /// mat = new osg::Material;
              // mat->setDataVariance( osg::Object::DYNAMIC);
              // pssDra->setAttribute( mat);
              //}
              //mat->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( 1, 0, 0, 0));
 
              //ֱ�Ӹı�����ɫ
              osg::ref_ptr< osg::Geometry> pGmt= picker->getFirstIntersection().drawable->asGeometry();
              if ( pGmt != NULL)
              {                
                  //��ɫ(�Ȼ�ȡԭʼ��ɫ���飬�ٵ���dirty()����)
                  osg::Vec4Array* pv4aColorsOld=  dynamic_cast< osg::Vec4Array*> ( pGmt->getColorArray());
                  if ( pv4aColorsOld != NULL)
                  {
                     pv4aColorsOld->clear();
                     pv4aColorsOld->push_back( osg::Vec4(  1.0f, 0.0f, 0.0f, 1.0f) );//��ɫ
 
                     pv4aColorsOld->dirty();//������ɫ����
                      pGmt->getOrCreateStateSet()->setMode( GL_LIGHTING,osg::StateAttribute::OFF|
                         osg::StateAttribute::OVERRIDE);//�رյƹ�
 
                     }
                  else
                  {
                     pv4aColorsOld= new osg::Vec4Array;
                     pv4aColorsOld->push_back( osg::Vec4(  1.0f, 0.0f, 0.0f, 1.0f) );//��ɫ
                     pGmt->setColorArray( pv4aColorsOld);
                      pGmt->setColorBinding( osg::Geometry::BIND_OVERALL/*BIND_PER_PRIMITIVE_SET:*/);
 
                      pGmt->getOrCreateStateSet()->setMode( GL_LIGHTING,osg::StateAttribute::OFF|osg::StateAttribute::OVERRIDE);//�رյƹ�
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
	os<<"MousePos(X: "<< m_vPosWindowMouse.x()<<",Y: "<< m_vPosWindowMouse.y()<<")";//����
	gdlist = os.str(); 
	setLabel(gdlist); 
}
osg::Node* createHUD_viewPoint( osgText::Text* text)
{
	//��������
	std::string font("fonts/arial.TTF");//�˴����õ��Ǻ������� "fonts/STCAIYUN.TTF"
	text->setFont( font); 
	//����������ʾ��λ��(����Ϊ(0,0),X�����ң�Y������)
	osg::Vec3 position( 100.0f, 10.0f,0.0f);
	text->setPosition(position);   
	text->setColor( osg::Vec4( 1, 1, 0, 1));
	text->setText(L"");//������ʾ������
	text->setCharacterSize(15);
	text->setDataVariance(osg::Object::DYNAMIC);//һ��Ҫ��������Ϊ��̬���������Ῠס���������������osgcatch��
	//������ڵ�
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable( text );//������Text����drawable���뵽Geode�ڵ���
	//����״̬
	osg::StateSet* stateset = geode->getOrCreateStateSet(); 
	stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);//�رյƹ�
	stateset->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);//�ر���Ȳ���
	//��GL_BLEND���ģʽ���Ա�֤Alpha������ȷ��
	stateset->setMode(GL_BLEND,osg::StateAttribute::ON);
	//���
	osg::Camera* camera = new osg::Camera;
	//����͸�Ӿ���
	camera->setProjectionMatrix(osg::Matrix::ortho2D(0,600,0,600));//����ͶӰ   
	//���þ��Բο�����ϵ��ȷ����ͼ���󲻻ᱻ�ϼ��ڵ�ı任����Ӱ��
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	//��ͼ����ΪĬ�ϵ�
	camera->setViewMatrix(osg::Matrix::identity());
	//���ñ���Ϊ͸��������Ļ���������ClearColor 
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setAllowEventFocus( false);//����Ӧ�¼���ʼ�յò�������
	//������Ⱦ˳�򣬱����������Ⱦ
	camera->setRenderOrder(osg::Camera::POST_RENDER); 
	camera->addChild(geode);//��Ҫ��ʾ��Geode�ڵ���뵽���
	return camera; 
};
void createGraphicsWindow( osgViewer::Viewer& viewer,int x, int y, int w, int h, const std::string& name, bool windowDecoration)
{
	//GraphicsContext�豸�����Ĺؼ�����
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits ;
	traits->windowName = name;
	traits->x = x; //��Ļ���Ͻ�Ϊ����ԭ�㣬����Ϊx������Ϊy��x��y���ǽ��濪ʼ�㣩
	traits->y = y; 
	traits->width = w;//����
	traits->height = h; //����
	traits->windowDecoration = windowDecoration; 
	traits->doubleBuffer = true;
	traits->sharedContext = 0; 
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
	gc->setClearColor( osg::Vec4f( 1.0f, 0.0f, 0.0f, 1.0f)); //��������windows������ɫ
	gc->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//�����
	osg::ref_ptr<osg::Camera> cameraMaster = viewer.getCamera();
	cameraMaster->setGraphicsContext(gc.get());//����GraphicsContext�豸������
	//����ӿ�����
	cameraMaster->setViewport(new osg::Viewport( 50, 50, traits->width, traits->height));/*���½�Ϊ����ԭ�㣬����Ϊx������Ϊy*/
}
int main( int argc, char **argv )
{
	osgViewer::Viewer viewer; 
	osg::ref_ptr<osg::Group> root= new osg::Group; 
	osg::ref_ptr< osg::Node> model = osgDB::readNodeFile("F:/3rdpart/OSG/OpenSceneGraph/data/fountain.osgt");// glider nathan
	osg::ref_ptr< osg::MatrixTransform> pmt= new osg::MatrixTransform();
	pmt->addChild( model.get());
	root->addChild( pmt.get());//����ĳ��ģ��
	osgText::Text* text = new osgText::Text; 
	root->addChild( createHUD_viewPoint( text));//����HUD����
	osg::ref_ptr< CHUD_viewPoint> pHUD= new CHUD_viewPoint( text);
	viewer.addEventHandler( pHUD.get());
	std::string name="first point";
	createGraphicsWindow(viewer,300,33,800,600,name,true);
	g_grpMouse= new osg::Group();
	//����״̬
	osg::StateSet* stateset = g_grpMouse->getOrCreateStateSet(); 
	stateset->setMode( GL_LIGHTING,osg::StateAttribute::OFF);//�رյƹ�
	stateset->setMode( GL_DEPTH_TEST,osg::StateAttribute::OFF);//�ر���Ȳ���
	root->addChild( g_grpMouse.get());
	//viewer.setUpViewInWindow( 100, 50, 600, 600);//���ô��ڴ�С��ʵ����createGraphicsWindow���Ƶ�Ч��
	viewer.setSceneData( root.get());
	viewer.realize();
	viewer.run() ;  
	return 0;
}