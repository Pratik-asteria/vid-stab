#include <iostream>
#include <stdlib.h>
using std::cout;
using std::endl;
int main() {
   
   const int size = 100;
   //double N[size] = {1, 2, 3, 4};
  for (int i=0, i< size, i++){
  N[i]= rand() % 10 + 1;
  }
   int l_size = 11;
   double sum = 0;
   double mAvg = 0;
 
   for (int i = 0; i <= (size - l_size); i++) {
      sum = 0;                
      cout << "Numbers ";    
      
      for (int j = i; j < i + l_size; j++) {
         sum += N[j];          
         cout << N[j] << " ";  
      }
      
      mAvg = sum / l_size;
      cout << endl << "Moving Average: " << mAvg << endl;
   }
   return 0;
}
