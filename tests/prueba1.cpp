#include <iostream>
#include <string>


bool find_string(std::string& cadena, const std::string& obj){
   size_t len_cad= cadena.length();
   size_t len_obj = obj.length();
   int aux_c1 = 0;
   bool aux_b1 = false;
   std::string buffer = "";

   for(int i=0; i<len_cad; i++){
      if(cadena[i] == obj[aux_c1] && !aux_b1){
         aux_b1 = true;
      };
      if(aux_b1){
         if(cadena[i] == obj[aux_c1]){                                                  buffer += cadena[i];
            aux_c1 += 1;
         } else {
            aux_b1 = false;
            aux_c1 = 0;
         };
      };

   };

   // Condicion final:
   if(buffer == obj){
      return true;
   }else {
      return false;
   };


};


int main(){

   std::string cadena = "Se ha comido 'Carlos' la empanada";
   std::string obj = "Carlos";
   bool booleano = find_string(cadena, obj);

   if(booleano){
      std::cout<<"Se ha encontrado: "<<obj<<" en la cadena"<<std::endl;
   } else {
      std::cout<<"NO se ha encontrado: "<<obj<<std::endl;
   };
   std::cout.flush();

   return 0;
};
