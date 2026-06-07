#include "Gauss_solve.h"
#include "util.h"

#include <exception>
#include <iostream>

int main(int argc, const char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " input.csv\n";
        return 1;
    }

    try
    {
        GaussMatrix ab = load_csv_to_matrix(argv[1]);
        GaussVector solution = Gauss_solve(ab);
        print_solution_csv(std::cout, solution);
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
