#include <cmath>
#include <cstdint>
#include <iomanip>
#include <fstream>

#include <GLApp.h>
#include <bmp.h>
#include <Grid2D.h>
#include <ostream>

class GLIPApp : public GLApp {
public:
  Image image;
    
  GLIPApp() : GLApp(512, 512, 4, "Image Processing")
  {
  }
  
  void toGrayscale(bool uniform=false) {
    // TODO: convert image to grayscale
    for (uint32_t y = 0;y<image.height;++y) {
      for (uint32_t x = 0;x<image.width;++x) {
        float r = image.getValue(x, y, 0);
        float g = image.getValue(x, y, 1);
        float b = image.getValue(x, y, 2);

        float val;
        if(uniform){
          val = r * 0.299f + g * 0.587f + b * 0.114f;
        } else {
          val = r / 3.0f + g / 3.0f + b / 3.0f;
        }

        image.setValue(x, y, 0, val);
        image.setValue(x, y, 1, val);
        image.setValue(x, y, 2, val);
      }
    }
  }

  void loadImage() {
    try {
      image = BMP::load("lenna.bmp");
    } catch (...) {
      image = Image(512,512);
      for (uint32_t y = 0;y<image.height;++y) {
        for (uint32_t x = 0;x<image.width;++x) {
          image.setNormalizedValue(x,y,0,float(x)/image.width);
          image.setNormalizedValue(x,y,1,float(y)/image.height);
          image.setNormalizedValue(x,y,2,0.5f);
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

    for (uint32_t y = image.height; y > 0; y -= 4) {
      for (uint32_t x = 0;x<image.width;x += 2) {
        float value = (image.getValue(x,y,0) + image.getValue(x,y,1) + image.getValue(x,y,2)) / 3.0f;
        int lookup =  std::floor((value / 255.0f * lut.length()));
        ss << lut[lookup];
      }
      ss << "\n";
    }

    return ss.str();
  }
  
  void filter(const Grid2D& filter) {
    // TODO: apply filter to image
    Image tempImage = Image(image);
    int filterWidth = filter.getWidth();
    int filterHeight = filter.getHeight();
    
    for (uint32_t y = 0;y<image.height;++y) {
      for (uint32_t x = 0;x<image.width;++x) {
        for(uint32_t c = 0; c < 3; c++){
          Grid2D tempMatrix = Grid2D(filterWidth, filterHeight);

          tempMatrix.setValue(0,0,tempImage.getValue((x - 1) % image.width, (y - 1) % image.height, c));
          tempMatrix.setValue(1,0,tempImage.getValue( x                   , (y - 1) % image.height, c));
          tempMatrix.setValue(2,0,tempImage.getValue((x + 1) % image.width, (y - 1) % image.height, c));

          tempMatrix.setValue(0,1,tempImage.getValue((x - 1) % image.width,  y    , c));
          tempMatrix.setValue(1,1,tempImage.getValue( x                   ,  y    , c));
          tempMatrix.setValue(2,1,tempImage.getValue((x + 1) % image.width,  y    , c));

          tempMatrix.setValue(0,2,tempImage.getValue((x - 1) % image.width, (y + 1) % image.height, c));
          tempMatrix.setValue(1,2,tempImage.getValue( x                   , (y + 1) % image.height, c));
          tempMatrix.setValue(2,2,tempImage.getValue((x + 1) % image.width, (y + 1) % image.height, c));

          float value = 0.0f;
          for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
              value += tempMatrix.getValue(i,j) * filter.getValue(j,i);
            }
          }

          image.setValue(x,y,c,std::sqrt(value * value)); 
        }
      }
    }

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
        case GLFW_KEY_E :
          filter({3,3, { -1, -1, -1,
                         -1,  8, -1,
                         -1, -1, -1 }});
          break;
        case GLFW_KEY_I :
          filter({3,3, { 0, 1, 0,
                         0, 1, 0,
                         0, 0, 0}});
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
        case GLFW_KEY_T : {
          std::ofstream file{ "ascii.txt" };
          file << toString() << std::endl;
          break;
        }
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
