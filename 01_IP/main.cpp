#include <iomanip>

#include <GLApp.h>
#include <bmp.h>
#include <Grid2D.h>

class GLIPApp : public GLApp {
public:
  Image image;
    
  GLIPApp() : GLApp(512, 512, 4, "Image Processing")
  {
  }
  
  void toGrayscale(bool uniform=false) {
    // TODO: convert image to grayscale
  }

  void loadImage() {
    try {
      image = BMP::load("lenna.bmp");
    } catch (...) {
      image = Image(512,512);
      for (uint32_t y = 0;y<image.height;++y) {
        for (uint32_t x = 0;x<image.width;++x) {
          const Vec3 rgb = Vec3::hsvToRgb({360*float(x)/image.width,float(y)/image.height,1.0f});
          image.setNormalizedValue(x,y,0,rgb.x());
          image.setNormalizedValue(x,y,1,rgb.y());
          image.setNormalizedValue(x,y,2,rgb.z());
          image.setValue(x,y,3,255);
        }
      }
    }
  }
  
  virtual void init() override {
    loadImage();
  }
      
  virtual void draw() override {
    drawImage(image);
  }
  
  
  std::string toString(bool bSmallTable=true) {
    const std::string lut1{"$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. "};
    const std::string lut2{"@%#*+=-:. "};
    const std::string& lut = bSmallTable ? lut2 : lut1;
    
    std::stringstream ss;

    // TODO: convert image to ASCII-Art

    return ss.str();
  }
  
  void filter(const Grid2D& filter) {
    // TODO: apply filter to image
  }
  
  virtual void keyboard(int key, int scancode, int action, int mods) override {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE :
          closeWindow();
          break;
        case GLFW_KEY_M :
          {
            Grid2D mean{3,3};
            mean.fill(1.0f/(mean.getHeight()*mean.getWidth()));
            filter(mean);
          }
          break;
        case GLFW_KEY_A :
          filter({3,3, {-1, 0, 1,
                        -2, 0, 2,
                        -1, 0, 1}});
          break;
        case GLFW_KEY_B:
          filter({3,3, {-1,-2,-1,
                         0, 0, 0,
                         1, 2, 1}});
        break;
        case GLFW_KEY_G :
          toGrayscale(false);
          break;
        case GLFW_KEY_H :
          toGrayscale(true);
          break;
        case GLFW_KEY_R :
          loadImage();
          break;
        case GLFW_KEY_T :
          std::cout << toString() << std::endl;
          break;
      }
    }
  }
  
};

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
#else
int main(int argc, char** argv) {
#endif
  try {
    GLIPApp imageProcessing;
    imageProcessing.run();
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
