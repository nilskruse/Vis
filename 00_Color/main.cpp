#include <GLApp.h>
#include <FontRenderer.h>

class MyGLApp : public GLApp {
public:
  Image image{640,480};
  FontRenderer fr{"helvetica_neue.bmp", "helvetica_neue.pos"};
  std::shared_ptr<FontEngine> fe{nullptr};
  std::string text;

  Vec3 convertPosToHSV(float x, float y) {
    // set HSV
    float H = x * 360.0f;
    float Sv = y;
    float V = 1.0f;

    //calculate C, H' and X
    float C = V * Sv;
    float Hs = H / 60.0f;

    float X = C * (1 - std::abs(std::fmod(Hs,2) - 1));

    //calculate (R_1, G_1, B_1) according to wikipedia 
    Vec3 rgb;
    if(0 <= Hs && Hs < 1) rgb = Vec3{C,X,0};
    if(1 <= Hs && Hs < 2) rgb = Vec3{X,C,0};
    if(2 <= Hs && Hs < 3) rgb = Vec3{0,C,X};
    if(3 <= Hs && Hs < 4) rgb = Vec3{0,X,C};
    if(4 <= Hs && Hs < 5) rgb = Vec3{X,0,C};
    if(5 <= Hs && Hs < 6) rgb = Vec3{C,0,X};

    //calculate m, add to rgb vector and return result
    float m = V - C;
    Vec3 mVec = Vec3{m,m,m};

    return rgb + mVec; 
  }
  
  virtual void init() {
    glEnv.setTitle("Color Picker");
    fe = fr.generateFontEngine();

    for (uint32_t y = 0;y<image.height;++y) {
      for (uint32_t x = 0;x<image.width;++x) {
        const Vec3 rgb = convertPosToHSV(float(x)/image.width, float(y)/image.height);
        image.setNormalizedValue(x,y,0,rgb.r);
        image.setNormalizedValue(x,y,1,rgb.g);
        image.setNormalizedValue(x,y,2,rgb.b);
        image.setValue(x,y,3,255);
      }
    }
    GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GL(glBlendEquation(GL_FUNC_ADD));
    GL(glEnable(GL_BLEND));
  }
  
  virtual void mouseMove(double xPosition, double yPosition) {
    Dimensions s = glEnv.getWindowSize();
    if (xPosition < 0 || xPosition > s.width || yPosition < 0 || yPosition > s.height) return;

    const Vec3 hsv{float(360*xPosition/s.width),float(1.0-yPosition/s.height),1.0f};
    const Vec3 rgb = convertPosToHSV(float(xPosition/s.width), float(1.0-yPosition/s.height));
    std::stringstream ss;
    ss << "HSV: " << hsv << "  RGB: " << rgb;
    text = ss.str();
  }
    
  virtual void draw() {
    drawImage(image);

    const Dimensions dim{ glEnv.getFramebufferSize() };
    fe->render(text, dim.aspect(), 0.03f, {0,-0.9f}, Alignment::Center, {0,0,0,1});
  }
};

#ifdef _WIN32
#include <Windows.h>
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
  try {
    MyGLApp myApp;
    myApp.run();
  }
  catch (const GLException& e) {
    std::stringstream ss;
    ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
    std::cerr << ss.str().c_str() << std::endl;
#else
    MessageBoxA(
      NULL,
      ss.str().c_str(),
      "OpenGL Error",
      MB_ICONERROR | MB_OK
    );
#endif
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
