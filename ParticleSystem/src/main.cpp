#include "ParticleSystemPCH.h"
#include "ElapsedTime.h"
#include "PivotCamera.h"
#include "ParticleEffect.h"
#include "SphereEmitter.h"
#include "CubeEmitter.h"
#include <algorithm>

PivotCamera g_Camera;
SphereEmitter g_ParticleEmitter;
CubeEmitter g_CubeEmitter;

#if _DEBUG
ParticleEffect g_ParticleEffect(1000);
#else
ParticleEffect g_ParticleEffect(1500);
#endif 

int g_iWindowWidth = 1280;
int g_iWindowHeight = 720;
int g_iGLUTWindowHandle = 0;
int g_iErrorCode = 0;

bool g_bLeftMouseDown = false;
bool g_bRightMouseDown = false;

bool g_bUpdate = true;

glm::vec2 g_MouseCurrent = glm::vec2(0);
glm::vec2 g_MousePrevious = glm::vec2(0);
glm::vec2 g_MouseDelta = glm::vec2(0);

glm::vec3 g_DefaultCameraTranslate( 0, 0, 100 );
glm::vec3 g_DefaultCameraRotate( 40, 0, 0 );
glm::vec3 g_DefaultCameraPivot( 0, 0, 0 );

void InitGL( int argc, char* argv[] );
void DisplayGL();
void IdleGL();
void SpecialKeysGL( int c, int x, int y );
void KeyboardGL( unsigned char c, int x, int y );
void MouseGL( int button, int state, int x, int y );
void MotionGL( int x, int y );
void ReshapeGL( int w, int h );

// We're exiting, cleanup the allocated resources.
void Cleanup();

int main( int argc, char* argv[] )
{
    InitGL( argc, argv );

    g_Camera.SetTranslate( g_DefaultCameraTranslate );
    g_Camera.SetRotate( g_DefaultCameraRotate );
    g_Camera.SetPivot( g_DefaultCameraPivot );

    if ( g_ParticleEffect.LoadTexture( "Data/Textures/Particle-Texture.png" ) )
    {
        std::cout << "Successfully loaded particle texture." << std::endl;
    }
    else
    {
        std::cerr << "Failed to load particle texture!" << std::endl;
    }
    ParticleEffect::ColorInterpolator colors;

    colors.AddValue(0.0f,  glm::vec4(0.96078,0.92157,0.00000,0.0) );     // red
	colors.AddValue(0.4f, glm::vec4(0.96078, 0.92157, 0.00000, 1));
	colors.AddValue(0.8f, glm::vec4(0.96078, 0.92157, 0.00000, 0.9));
	colors.AddValue(0.11f, glm::vec4(0.98824,0.67843, 0.11373, 0.8));
    colors.AddValue(0.15f, glm::vec4(0.98824, 0.67843, 0.11373, 0.7) );     // magenta
    colors.AddValue(0.20f, glm::vec4(0.98824, 0.67843, 0.11373, 0.66) );
	colors.AddValue(0.3f, glm::vec4(0.98824, 0.67843, 0.11373, 0.55));
	colors.AddValue(0.4f, glm::vec4(0.94510, 0.37255, 0.18039, 0.50));
    colors.AddValue(0.5f,  glm::vec4(0.94510 ,0.37255 ,0.18039, 0.20) );     // cyan
    colors.AddValue(0.67f, glm::vec4(0.94510 ,0.37255 ,0.18039, 0.15) );  // green
    colors.AddValue(0.84f, glm::vec4(0.76471 , 0.76471, 0.76471, 0.10) );   // yellow
    colors.AddValue(1.0f,  glm::vec4(0.76471, 0.76471, 0.76471, 0.05) );

	g_ParticleEffect.SetColorInterplator( colors );

    g_ParticleEffect.SetParticleEmitter(&g_ParticleEmitter);
    g_ParticleEffect.EmitParticles();
    g_ParticleEffect.SetCamera( &g_Camera );

    glutMainLoop();
}

void Cleanup()
{
    if ( g_iGLUTWindowHandle != 0 )
    {
        glutDestroyWindow( g_iGLUTWindowHandle );
        g_iGLUTWindowHandle = 0;
    }
}


void InitGL( int argc, char* argv[] )
{
    std::cout << "Initialise OpenGL..." << std::endl;

    glutInit(&argc, argv);
    int iScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int iScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    glutInitDisplayMode( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH );

    glutInitWindowPosition( ( iScreenWidth - g_iWindowWidth ) / 2,
        ( iScreenHeight - g_iWindowHeight ) / 2 );
    glutInitWindowSize( g_iWindowWidth, g_iWindowHeight );

    g_iGLUTWindowHandle = glutCreateWindow( "OpenGL" );

    // Register GLUT callbacks
    glutDisplayFunc(DisplayGL);
    glutIdleFunc(IdleGL);
    glutMouseFunc(MouseGL);
    glutMotionFunc(MotionGL);
    glutSpecialFunc(SpecialKeysGL);
    glutKeyboardFunc(KeyboardGL);
    glutReshapeFunc(ReshapeGL);

    // Setup initial GL State
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepth( 1.0f );

    glShadeModel( GL_SMOOTH );

    std::cout << "Initialise OpenGL: Success!" << std::endl;
}

void DrawAxis( float fScale, glm::vec3 translate = glm::vec3(0) )
{
    glPushAttrib( GL_ENABLE_BIT );

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );

    glPushMatrix();
    glTranslatef( translate.x, translate.y, translate.z );
    glScalef( fScale, fScale, fScale );
    glColor3f( 0.0f, 0.0f, 1.0f );

    glBegin( GL_LINES );
    {
        glColor3f( 1.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 0.0 );
        glVertex3f( 1.0f, 0.0f, 0.0f );

        glColor3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 1.0f, 0.0f );

        glColor3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( 0.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 0.0f, 1.0f );
    }
    glEnd();

    glPopMatrix();
    glPopAttrib();
	glEnd();
}

inline void Quad(GLdouble *v1, GLdouble *v2, GLdouble *v3, GLdouble *v4)
{
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3dv(v1);
	glTexCoord2f(1, 0);
	glVertex3dv(v2);
	glTexCoord2f(1, 1);
	glVertex3dv(v3);
	glTexCoord2f(0, 1);
	glVertex3dv(v4);
	glEnd();
}

void Draw_polygon()
{
	GLdouble a[] = { 0., 0., 10. };
	GLdouble b[] = { 2., 0., 10. };
	GLdouble c[] = { 2., 2., 10. };
	GLdouble d[] = { 0., 2., 10. };
	GLdouble e[] = { 0., 0., 0. };
	GLdouble f[] = { 2., 0., 0. };
	GLdouble g[] = { 2., 2., 0. };
	GLdouble h[] = { 0., 2., 0. };

	glEnable(GL_TEXTURE_2D);
	g_ParticleEffect.LoadTexture("Data/Textures/plank01.bmp");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	Quad(a, b, c, d); // Front
	Quad(c, b, f, g); // Right
	Quad(h, g, f, e); // Back
	Quad(d, h, e, a); // Left
	Quad(d, c, g, h); // Top
	Quad(e, f, b, a); // Bottom

	glDisable(GL_TEXTURE_2D);
}


void DisplayGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );                   // Clear the color buffer, and the depth buffer.

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    g_Camera.ApplyViewTransform();


	glPushMatrix();
	glRotated(30., 0., 50., 1.);
	Draw_polygon();
	glPopMatrix();


	glPushMatrix();
	glRotated(10., 0., 50., 1.);
	glTranslated(-1.5, 0, 0);
	Draw_polygon();
	glPopMatrix();

	glPushMatrix();
	glRotated(-20., 0., 50., 1.);
	glTranslated(-2, 0, 1);
	Draw_polygon();
	glPopMatrix();

	glPushMatrix();
	glRotated(-40., 0., 50., 1.);
	glTranslated(-2.5, 0, 1.3);
	Draw_polygon();
	glPopMatrix();

	g_ParticleEffect.LoadTexture("Data/Textures/Particle-Texture.png");
    g_ParticleEffect.Render();
	
    glutSwapBuffers();
    glutPostRedisplay();
}

void IdleGL()
{
    static ElapsedTime elapsedTime;
    float fDeltaTime = elapsedTime.GetElapsedTime();

    if ( g_bUpdate )
    {
        g_ParticleEffect.Update(fDeltaTime);
    }
    else 
    {
        g_ParticleEffect.BuildVertexBuffer();
    }

    glutPostRedisplay();
}

template< typename T >
void Increment( T& value, const T& step, const T& max )
{
    value = std::min( max, value + step );
}

template< typename T >
void Decrement( T& value, const T& step, const T& min )
{
    value = std::max( min, value - step );
}

void SpecialKeysGL( int c, int x, int y )
{
    const float fStep = 1.0f;
    switch ( c )
    {
    case GLUT_KEY_UP:
        {
            Increment( g_CubeEmitter.MaxWidth, fStep, 100.0f );
            Increment( g_CubeEmitter.MaxHeight, fStep, 100.0f );
            Increment( g_CubeEmitter.MaxDepth, fStep, 100.0f );
            g_CubeEmitter.MinWidth = -g_CubeEmitter.MaxWidth;
            g_CubeEmitter.MinHeight= -g_CubeEmitter.MaxHeight;
            g_CubeEmitter.MinDepth = -g_CubeEmitter.MaxDepth;

            Increment( g_ParticleEmitter.MinInclination, fStep, 180.0f );
            Decrement( g_ParticleEmitter.MaxInclination, fStep, g_ParticleEmitter.MinInclination );
        }
        break;
    case GLUT_KEY_DOWN:
        {
            Decrement( g_CubeEmitter.MaxWidth, fStep, 0.0f );
            Decrement( g_CubeEmitter.MaxHeight, fStep, 0.0f );
            Decrement( g_CubeEmitter.MaxDepth, fStep, 0.0f );

            g_CubeEmitter.MinWidth = -g_CubeEmitter.MaxWidth;
            g_CubeEmitter.MinHeight = -g_CubeEmitter.MaxHeight;
            g_CubeEmitter.MinDepth = -g_CubeEmitter.MaxDepth;

            Decrement( g_ParticleEmitter.MinInclination, fStep, 0.0f );
            Increment( g_ParticleEmitter.MaxInclination, fStep, 180.0f );
        }
        break;
    case GLUT_KEY_LEFT:
        {
            Increment( g_ParticleEmitter.MinAzimuth, fStep, 360.0f );
            Decrement( g_ParticleEmitter.MaxAzimuth, fStep, 0.0f );
        }
        break;
    case GLUT_KEY_RIGHT:
        {
            Decrement(g_ParticleEmitter.MinAzimuth, fStep, 0.0f );
            Increment(g_ParticleEmitter.MaxAzimuth , fStep, 360.0f );
        }
        break;
    };

    glutPostRedisplay();
}

void KeyboardGL( unsigned char c, int x, int y )
{
    float fRadiusStep = 1.0f;

    switch ( c )
    {
    case ' ': // Space bar
        {
            // Toggle updating of the simulation
            g_bUpdate = !g_bUpdate;
        }
        break;
    case '-':
        {
            g_ParticleEmitter.MaximumRadius -= fRadiusStep;
            g_ParticleEmitter.MinimumRadius -= fRadiusStep;

            g_ParticleEmitter.MaximumRadius = std::max( 0.0f, g_ParticleEmitter.MaximumRadius );
            g_ParticleEmitter.MaximumRadius = std::max( 0.0f, g_ParticleEmitter.MaximumRadius );
        }
        break;
    case '+':
        {
            g_ParticleEmitter.MaximumRadius += fRadiusStep;
            g_ParticleEmitter.MinimumRadius += fRadiusStep;

            g_ParticleEmitter.MaximumRadius = std::min( 200.0f, g_ParticleEmitter.MaximumRadius );
            g_ParticleEmitter.MaximumRadius = std::min( 200.0f, g_ParticleEmitter.MaximumRadius );
        }
        break;
    case 's':
    case 'S':
        {
            std::cout << "Shade Model: GL_SMOOTH" << std::endl;
            // Switch to smooth shading model
            glShadeModel( GL_SMOOTH );
        }
        break;
    case 'f':
    case 'F':
        {
            std::cout << "Shade Model: GL_FLAT" << std::endl;
            // Switch to flat shading model
            glShadeModel( GL_FLAT );
        }
        break;
    case 'r':
    case 'R':
        {
            std::cout << "Reset Parameters..." << std::endl;
            g_Camera.SetTranslate( g_DefaultCameraTranslate );
            g_Camera.SetRotate( g_DefaultCameraRotate );
            g_Camera.SetPivot( g_DefaultCameraPivot );
        }
        break;
    case '\033': // escape quits
    case '\015': // Enter quits    
    case 'Q':    // Q quits
    case 'q':    // q (or escape) quits
        {
            // Cleanup up and quit
            exit(0);
        }
        break;
    }

    glutPostRedisplay();
}

void MouseGL( int button, int state, int x, int y )
{
    if ( button == GLUT_LEFT_BUTTON )
    {
        g_bLeftMouseDown = ( state == GLUT_DOWN );
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        g_bRightMouseDown = ( state == GLUT_DOWN );
    }
    g_MousePrevious = glm::vec2( x, y );

}

void MotionGL( int x, int y )
{
    g_MouseCurrent = glm::vec2( x, y );
    g_MouseDelta = ( g_MousePrevious - g_MouseCurrent );

    // Update the camera
    if ( g_bLeftMouseDown && g_bRightMouseDown )
    {
        g_Camera.TranslateZ( g_MouseDelta.y );
    }
    else if ( g_bLeftMouseDown )
    {
        g_Camera.AddPitch( -g_MouseDelta.y );
        g_Camera.AddYaw( -g_MouseDelta.x );
    }
    else if ( g_bRightMouseDown )
    {
        g_Camera.TranslatePivotX( g_MouseDelta.x );
        g_Camera.TranslatePivotY( -g_MouseDelta.y );
    }    

    g_MousePrevious = g_MouseCurrent;
}

void ReshapeGL( int w, int h )
{
    std::cout << "ReshapGL( " << w << ", " << h << " );" << std::endl;

    h = std::max( h, 1 );
    
    g_iWindowWidth = w;
    g_iWindowHeight = h;

    g_Camera.SetViewport( 0, 0, w, h );
    g_Camera.ApplyViewport();

    g_Camera.SetProjection( 60.0f, w/(float)h, 0.1f, 1000.0f );
    g_Camera.ApplyProjectionTransform();

    glutPostRedisplay();
}

