#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <zbar.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <time.h>

using namespace std;
using namespace cv;
using namespace zbar;


string LecturaDeCodigo(int tiempoCamara) {



VideoCapture cap(1); // captura desde camara (0:compu , 1: externa)

    if (!cap.isOpened()) {
        cout << "No se pudo abrir la camara" << endl;
        return 0;
    }

    //INICIALIZACION DE COSAS
    namedWindow("Camara",CV_WINDOW_AUTOSIZE); // creacion de ventana "Camara"

    // Parte nueva para el ZBAR
    ImageScanner scanner;           // inicializacion de codigo escaneado
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);  // configuracion loca

    time_t tiempo;
    clock_t t_ini;
    clock_t t_fin;

    string Codigo = "";

    //EMPIEZA LA LECTURA CONTINUA DE LA CAM
    while (1) {
        Mat frame;
        bool bSuccess = cap.read(frame); // lectura de la cam Si o No
        if (!bSuccess) {
            cout << "Problemas con la captura de video" << endl;    //PROBAR CON LA CAMARA VIEJA!!!!
            break;
        }

        Mat OptImagen;
        cvtColor(frame,OptImagen,CV_BGR2GRAY);   // Cambio a escala de grises para mejor deteccion del codifo
        uchar *raw = (uchar *)OptImagen.data;

        Image image(frame.cols, frame.rows, "Y800", raw, frame.cols * frame.rows);

        scanner.scan(image);    // Empieza la magia, funcion .scan de zlib busca si detecta un codigo

        for(Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
            vector<Point> vp;


            //Codigo = symbol->get_data();

            if(Codigo != Codigo) t_ini = clock();       // Si lee un codigo distinto al anterior se reinicia el conteo

            t_fin = clock();
            tiempo = t_fin - t_ini;
            tiempo = ((float)tiempo)/CLOCKS_PER_SEC;

            if (tiempo > tiempoCamara) {
                Codigo = symbol->get_data();  // Si la lectura es constante por 'tiempoCam' da el dato leido, sino da dato en blanco
                return Codigo;
                }
            else if (tiempo < tiempoCamara) Codigo = "";

            // detecta que tipo de codigo y el codigo
            cout << "TIPO " << symbol->get_type_name() << " CODIGO \"" << Codigo << '"' <<" "<< endl;
            int n = symbol->get_location_size();
            for(int i=0;i<n;i++){
                vp.push_back(Point(symbol->get_location_x(i),symbol->get_location_y(i)));
            }

            RotatedRect r = minAreaRect(vp);    // Tambien acepta un cierto grado de roation

            // Encerrando con un rectangulo el codigo

            Point2f pts[4];    // 4: 4 esquinas del rect
            r.points(pts);
            for(int i=0;i<4;i++){                  //  B  G  R
                line(frame,pts[i],pts[(i+1)%4],Scalar(255,0,255),3);   // se forman las lineas a color magenta B 255 Y R 255
            }
        } // FIN del for GRANDE

        imshow("Camara", frame);           // Muestra la captura de la camara actual

        char ch =  cvWaitKey(25);   // 25 ms
        if(ch==27) break;           // Esc salida

       // if(ch=='s') {               // S Salvar imagen
        //    cvSaveImage("out.jpg",frame);
        //}

   }

    // Limpieza de todo
    cvDestroyWindow("Camara");

    return 0;


}


int main(int argc, char* argv[]) {

string cod = LecturaDeCodigo(4);

cout << cod << endl;
return 0;
}
