#include <iostream>
#include "TextClassifier.h"

int main(int num_args, char **args ) {

    if (num_args == 2){
        TextClassifier tc(args[1]);
    }else if (num_args == 3){
        TextClassifier tc(args[1], args[2]);
    }
    return 0;
}
