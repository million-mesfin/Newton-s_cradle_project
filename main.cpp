#include <math.h>
#include <GL/glut.h>

const double PI = 3.141592653589793;

GLUquadricObj *quadratic;   // Necessary for drawing cylinders with glut
int milliseconds = 20;      // Time between each screen update
float maxAngle = 50;        // Maximum value for the angle
float maxIncrement = 6.5;   // Maximum value for the angle increments
float angle = -maxAngle;    // The left sphere will start suspended at the maximum valid angle
bool clockwise = false;   // and will move in a counter-clockwise direction
// Spheres
int totalSpheres = 6;       // Total number of spheres
int movingSpheres = 4;      // Number of spheres that will be in motion
float sphereDiameter = 1.0; // Diameter of each sphere
float cubeSize = 0.125;     // Size of the cube embedded in each sphere
float sphereBaseDistance = 1.0; // Distance between the spheres and the base when the angle is 0
// Tubes
float tubeRadius = 0.125;   // Radius of each cylinder
GLint tubeSlices = 62;      // Number of subdivisions around the circumference of the cylinder
GLint tubeStacks = 32;      // Number of subdivisions along the height of the cylinder
float tubeWidth = 7.;      // Size of the rectangular frame formed by the tubes (including the diameter of each tube)
float tubeHeight = 5;
float tubeDepth = 4.5;
// The length of the string is calculated based on the size of the tubes,
// the diameter of the spheres, and the distance between the spheres and the base
float stringLength = tubeHeight - sphereBaseDistance - sphereDiameter - cubeSize / 2;
// Camera initial positions
float cameraPosX = 0.0;
float cameraPosY = 0.0;
float cameraPosZ = 15.0;
float cameraRotX = 0.0;
float cameraRotY = 0.0;
float cameraRotZ = 0.0;

//Initialize the rendering settings
void initRendering() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);         //Enable vector normal normalization for lighting calculations
    glEnable(GL_COLOR_MATERIAL);    //Color material tracking for lighting in different directions
    glShadeModel(GL_SMOOTH);        //Creates smooth surfaces by averaging normals across the surface

    quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);       // Determine the orientation of a surface at a particular point and averaged for smoothness
}
// Camera position and rotation
void positionCamera() {
    glTranslatef(cameraPosX, cameraPosY, -cameraPosZ);      //Set initial position of camera
    glRotatef(cameraRotX, 1.0, 0.0, 0.0);
    glRotatef(cameraRotY, 0.0, 1.0, 0.0);
    glRotatef(cameraRotZ, 0.0, 0.0, 1.0);
}
// Convert degrees to radians
double toRadians(double degree) {
    return degree * PI / 180;
}
// Draws a base with various faces (top, bottom, left, right, front, back)
void drawBase() {
    // Dimensions for the base
    float baseWidth = 8.0;
    float baseHeight = 1.0;
    float baseDepth = 6.0;

    glPushMatrix();     //Save the current transformation matrix
    glTranslatef(0.0f, -(sphereDiameter / 2 + sphereBaseDistance + baseHeight / 2), 0.0f);
    glColor3f(0.145f, 0.69f, 0.573f); // Setting the color for the base
    glBegin(GL_QUADS);
        //Top
        glNormal3f(0.0, 1.0f, 0.0f);    //Set the normal vector along the y-axis to determine the direction of surface
        glVertex3f(-baseWidth / 2, baseHeight / 2, -baseDepth / 2);
        glVertex3f(baseWidth / 2, baseHeight / 2, -baseDepth / 2);
        glVertex3f(baseWidth / 2, baseHeight / 2, baseDepth / 2);
        glVertex3f(-baseWidth / 2, baseHeight / 2, baseDepth / 2);
        //Bottom
        glNormal3f(0.0, -1.0f, 0.0f);
        glVertex3f(-baseWidth / 2, -baseHeight / 2, -baseDepth / 2);
        glVertex3f(baseWidth / 2, -baseHeight / 2, -baseDepth / 2);
        glVertex3f(baseWidth / 2, -baseHeight / 2, baseDepth / 2);
        glVertex3f(-baseWidth / 2, -baseHeight / 2, baseDepth / 2);
        //Left
        glNormal3f(-1.0, 0.0f, 0.0f);
        glVertex3f(-baseWidth / 2, -baseHeight / 2, -baseDepth / 2);
        glVertex3f(-baseWidth / 2, -baseHeight / 2, baseDepth / 2);
        glVertex3f(-baseWidth / 2, baseHeight / 2, baseDepth / 2);
        glVertex3f(-baseWidth / 2, baseHeight / 2, -baseDepth / 2);
        //Right
        glNormal3f(1.0, 0.0f, 0.0f);
        glVertex3f(baseWidth / 2, -baseHeight / 2, -baseDepth / 2);
        glVertex3f(baseWidth / 2, -baseHeight / 2, baseDepth / 2);
        glVertex3f(baseWidth / 2, baseHeight / 2, baseDepth / 2);
        glVertex3f(baseWidth / 2, baseHeight / 2, -baseDepth / 2);
        //Front
        glNormal3f(0.0, 0.0f, 1.0f);
        glVertex3f(-baseWidth / 2, -baseHeight / 2, baseDepth / 2);
        glVertex3f(baseWidth / 2, -baseHeight / 2, baseDepth / 2);
        glVertex3f(baseWidth / 2, baseHeight / 2, baseDepth / 2);
        glVertex3f(-baseWidth / 2, baseHeight / 2, baseDepth / 2);
        //Back
        glNormal3f(0.0, 0.0f, -1.0f);
        glVertex3f(-baseWidth / 2, -baseHeight / 2, -baseDepth / 2);
        glVertex3f(baseWidth / 2, -baseHeight / 2, -baseDepth / 2);
        glVertex3f(baseWidth / 2, baseHeight / 2, -baseDepth / 2);
        glVertex3f(-baseWidth / 2, baseHeight / 2, -baseDepth / 2);
    glEnd();
    glPopMatrix();      //Restore the previous transformation matrix
}
// Draw the tubes
void drawTubes() {
    glPushMatrix();
    glTranslatef(0.0f, tubeHeight / 2 - sphereDiameter / 2 - sphereBaseDistance, 0.0f); // Geometric center

    glColor3f(0.69f, 0.69f, 0.69f);
    // Lighting
    GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };     //Not in  a direct light
    GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };     //In direct light
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };    //Object highlights
    GLfloat mat_shininess[] = { 100.0 };                //Sharpness of object highlights
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // Front Top Tube
    glPushMatrix();
    glTranslatef(-tubeWidth / 2, tubeHeight / 2 - tubeRadius * 2, tubeDepth / 2 - tubeRadius);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadratic, tubeRadius, tubeRadius, tubeWidth, tubeSlices, tubeStacks);
    glPopMatrix();

    // Back Top Tube
    glPushMatrix();
    glTranslatef(-tubeWidth / 2, tubeHeight / 2 - tubeRadius * 2, -(tubeDepth / 2 - tubeRadius));
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(quadratic, tubeRadius, tubeRadius, tubeWidth, tubeSlices, tubeStacks);
    glPopMatrix();

    // Front Left Tube
    glPushMatrix();
    glTranslatef(-(tubeWidth / 2 - tubeRadius), tubeHeight / 2 - tubeRadius, tubeDepth / 2 - tubeRadius);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadratic, tubeRadius, tubeRadius, tubeHeight - tubeRadius, tubeSlices, tubeStacks);
    glPopMatrix();

    // Back Left Tube
    glPushMatrix();
    glTranslatef(-(tubeWidth / 2 - tubeRadius), tubeHeight / 2 - tubeRadius, -(tubeDepth / 2 - tubeRadius));
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadratic, tubeRadius, tubeRadius, tubeHeight - tubeRadius, tubeSlices, tubeStacks);
    glPopMatrix();

    // Front Right Tube
    glPushMatrix();
    glTranslatef((tubeWidth / 2 - tubeRadius), tubeHeight / 2 - tubeRadius, tubeDepth / 2 - tubeRadius);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadratic, tubeRadius, tubeRadius, tubeHeight - tubeRadius, tubeSlices, tubeStacks);
    glPopMatrix();

    // Back Right Tube
    glPushMatrix();
    glTranslatef((tubeWidth / 2 - tubeRadius), tubeHeight / 2 - tubeRadius, -(tubeDepth / 2 - tubeRadius));
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    gluCylinder(quadratic, tubeRadius, tubeRadius, tubeHeight - tubeRadius, tubeSlices, tubeStacks);
    glPopMatrix();

    glPopMatrix();
}
// Draw the sphere attached to a cube
void drawTetheredSphere(float angle) {
    glPushMatrix();

    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glTranslatef(0.0f, -stringLength, 0.0f);

    glColor3f(0.8, 0.8, 0.8);
    // Lighting
    GLfloat mat_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
    GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 300.0 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    // Draw Cube for Tethering
    glutSolidCube(cubeSize);
    // Draw Sphere
    glPushMatrix();
    glTranslatef(0.0f, -(sphereDiameter / 2), 0.0f);    //Position the sphere below the cube
    glutSolidSphere(sphereDiameter / 2, 20, 20);       //Sphere radius, slice, stack
    glPopMatrix();

    glRotatef(-angle, 0.0f, 0.0f, 1.0f);
    // Draw Strings
    float distX = sin(toRadians(angle)) * stringLength;
    float distY = cos(toRadians(angle)) * stringLength;
    float distZ = tubeDepth / 2 - tubeRadius;

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        // Towards back tube
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-distX, distY, -distZ);
        // Towards front tube
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-distX, distY, distZ);
    glEnd();

    glPopMatrix();
}

// Draw spheres
void drawSpheres() {
    glPushMatrix();
    glTranslatef(0.0f, tubeHeight - tubeRadius - sphereBaseDistance - sphereDiameter / 2, 0.0f); // Tube height

    for (int i = 1; i <= totalSpheres; i++) {
        glPushMatrix();
        glTranslatef(-totalSpheres / 2.0f - sphereDiameter / 2.0f + i * sphereDiameter, 0.0f, 0.0f); // Center in X
        if (i <= movingSpheres && angle < 0)
            drawTetheredSphere(angle);
        else if (i > totalSpheres - movingSpheres && angle > 0)
            drawTetheredSphere(angle);
        else
            drawTetheredSphere(0);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);     //Select the model-view as the current matrix
    glLoadIdentity();   //reset the model-view matrix to its default state

    // Lighting
    GLfloat ambientLight[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuseLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPos[] = {-cameraPosX + tubeWidth, -cameraPosY + tubeHeight, -cameraPosZ + tubeDepth, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    // Viewing
    positionCamera();

    drawBase();
    drawTubes();
    drawSpheres();

    glutSwapBuffers();
}
// Calculate angle and direction for rotation and display
void update(int value) {
    float increment = maxIncrement - abs(angle) / maxAngle * maxIncrement * 0.85;
    if (clockwise && angle <= -maxAngle) {
        clockwise = false;
    } else if (!clockwise && angle >= maxAngle) {
        clockwise = true;
    }
    if (clockwise)
        angle -= increment;
    else
        angle += increment;

    glutPostRedisplay();
    glutTimerFunc(milliseconds, update, 0);
}
// Move camera using arrow keys
void handleSpecialKeys(int key, int x, int y) {
    int inc = 2.0;
    switch (key) {
        case GLUT_KEY_RIGHT:
            cameraRotY += inc;
            glutPostRedisplay();
            break;
        case GLUT_KEY_LEFT:
            cameraRotY -= inc;
            glutPostRedisplay();
            break;
        case GLUT_KEY_UP:
            cameraRotX -= inc;
            glutPostRedisplay();
            break;
        case GLUT_KEY_DOWN:
            cameraRotX += inc;
            glutPostRedisplay();
            break;
    }
}
// Handle window resize
void handleResize(int w, int h) {
    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 200.0);

    // ViewPort
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);

    glutCreateWindow("Newton's Cradle");
    initRendering();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(handleResize);
    glutSpecialFunc(handleSpecialKeys);
    glutTimerFunc(milliseconds, update, 0);

    glutMainLoop();
    return 0;
}
