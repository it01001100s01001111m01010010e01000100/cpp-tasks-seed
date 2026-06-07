#include "util.h"

#include <cctype>
#include <cstddef>
#include <iomanip>
#include <lazycsv.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
bool parse_double(const std::string& text, double& value)
{
    try
    {
        std::size_t pos = 0;
        value = std::stod(text, &pos);

        while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])) != 0)
        {
            ++pos;
        }

        return pos == text.size();
    }
    catch (const std::exception&)
    {
        return false;
    }
}
} // namespace

GaussMatrix load_csv_to_matrix(const char *filename)
{
    std::vector<std::vector<double>> rows;
    lazycsv::parser parser{filename};

    for (const auto row : parser)
    {
        std::vector<double> values;
        bool numeric_row = true;

        for (const auto cell : row)
        {
            double value = 0.0;

            if (!parse_double(std::string(cell.raw()), value))
            {
                numeric_row = false;
                break;
            }

            values.push_back(value);
        }

        if (numeric_row && !values.empty())
        {
            rows.push_back(values);
        }
    }

    if (rows.empty())
    {
        throw std::runtime_error("CSV does not contain numeric data");
    }

    const std::size_t cols = rows[0].size();

    for (const auto& row : rows)
    {
        if (row.size() != cols)
        {
            throw std::runtime_error("CSV table must be rectangular");
        }
    }

    if (cols != rows.size() + 1)
    {
        throw std::runtime_error("Expected augmented matrix with N rows and N + 1 columns");
    }

    GaussMatrix matrix(static_cast<Eigen::Index>(rows.size()), static_cast<Eigen::Index>(cols));

    for (Eigen::Index i = 0; i < matrix.rows(); ++i)
    {
        for (Eigen::Index j = 0; j < matrix.cols(); ++j)
        {
            matrix(i, j) = rows[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)];
        }
    }

    return matrix;
}

void print_matrix_as_csv(std::ostream& out, const GaussMatrix& matrix, int prec)
{
    for (Eigen::Index j = 0; j < matrix.cols(); ++j)
    {
        if (j > 0)
        {
            out << ',';
        }

        if (j + 1 == matrix.cols())
        {
            out << 'B';
        }
        else
        {
            out << 'A' << j;
        }
    }

    out << '\n' << std::fixed << std::setprecision(prec);

    for (Eigen::Index i = 0; i < matrix.rows(); ++i)
    {
        for (Eigen::Index j = 0; j < matrix.cols(); ++j)
        {
            if (j > 0)
            {
                out << ',';
            }

            out << matrix(i, j);
        }

        out << '\n';
    }
}

void print_solution_csv(std::ostream& out, const GaussVector& solution, int prec)
{
    out << "x\n" << std::fixed << std::setprecision(prec);

    for (Eigen::Index i = 0; i < solution.size(); ++i)
    {
        out << solution(i) << '\n';
    }
}
