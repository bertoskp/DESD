
#include "SimpleNetworkIO.h"
#include "DictionaryIO.h"
#include "BehavioralSpaceIO.h"
#include "AbduceIO.h"


class File{
public:
    
    //verifica che una cartella o file eista  o meno
    static bool path_exists(const std::string& path_src) {
        const  std::filesystem::path path = path_src;
        return exists(path);
    }
    
    
    // leggere i file quando si carica un progetto
    static void readFileInProject(bool online, std::string projName, int space){
     std::string path;
     std::size_t psub=projName.find("/");
     std::string sub=projName.substr(psub+1) ;
    
     switch (space) {
     case 1:{
         path=path+projName+"/bs_"+sub+".json";
         auto bs = BehavioralSpaceIO::loadOffline(path);
       
       
         break;
     }
     case 2:
         {
             path=path+projName+"/bsd_"+sub+".json";
         auto lbh = BehavioralSpaceIO::loadOffline(path);
            
             break;
         }
     case 3:
         {
         path=path+projName+"/dic_"+sub+".json";
          auto dictionary = DictionaryIO::loadOffline(path);
         break;
         }
     default:
         ;
     }
    }
    
    
    
    static std::vector<std::string> get_directories(const std::string& s) {
        std::vector<std::string> r;
        for(auto& p : std::filesystem::directory_iterator(s))
            if(p.status().type() == std::filesystem::file_type::directory)
                r.push_back(p.path().string());
        return r;
    }
    
    
    
    // copia un file in una directory specificata
    //utilizzato per  copiare la rete e rinominarla con il nome del progetto all interno della cartella che verra creata
    static void copyfile( std::filesystem::path sourceFile ,    std::filesystem::path targetParent,std::string name){
        
        auto target = targetParent.u8string()+ "/"+ name+".json"; // sourceFile.filename() returns "sourceFile.ext".

        try // If you want to avoid exception handling, then use the error code overload of the following functions.
        {
            std::filesystem::create_directories(targetParent); // Recursively create target directory if not existing.
            std::filesystem::copy_file(sourceFile, target, std::filesystem::copy_options::overwrite_existing);
        }
        catch (std::exception& e) // Not using fs::filesystem_error since std::bad_alloc can throw too.
        {
            std::cout << e.what();
        }
    }

};
