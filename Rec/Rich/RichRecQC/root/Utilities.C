
#include "Utilities.h"

namespace Rich
{
  namespace Utils
  {

    // just use canvas method
    void systemSaveImage ( TCanvas * canvas,
                           const std::string & name )
    {
      std::cout << "Creating image : " << name << std::endl;
      canvas->SaveAs( name.c_str() );
    }

    /*
    // method to save image files from a canvas
    // uses linux "convert" command as built in image creation isn't very good
    void systemSaveImage ( TCanvas * canvas,
    const std::string & name )
    {

    // Find "." in file name
    const int dot = name.find_first_of(".");
    // create file type
    const std::string fileType = name.substr(dot+1);

    // if requested type is eps, just do that
    if ( "eps" == fileType )
    {
    // create eps file
    canvas->SaveAs( name.c_str() );
    }
    else
    {

    //Use convert as it seems better than built in root image files
    // create basename
    const std::string baseName = name.substr(0,dot);
    // crate temp eps file
    const std::string tmpEps = baseName+"_temp.eps";
    canvas->SaveAs( tmpEps.c_str() );
    // otherwise, convert eps to requested type using system convert command
    // this only works on linux systems with the "convert" command installed
    const std::string command = "convert "+tmpEps+" "+name;
    std::cout << "Rich::Utils::systemSaveImage " << command << std::endl;
    gSystem->Exec( command.c_str() );
    // delete temp eps file
    gSystem->Exec( ("rm "+tmpEps).c_str() );

    }

    }
    */

  }
}
