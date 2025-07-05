// Copyright (C) 2024 Max Planck Institute for Dynamics of Complex Technical Systems, Magdeburg
//
// This file is part of phgasnets
//
// SPDX-License-Identifier:  GPL-3.0-or-later

# include "utils.hpp"
# include <fstream>
# include <iomanip>

Eigen::VectorXd phgasnets::verticallyBlockVectors(
    const std::vector<Eigen::VectorXd>& vectors
){
    int total_rows = 0;
    for (const auto& vector : vectors) {
        total_rows += vector.rows();
    }

    Eigen::VectorXd result(total_rows, 1);
    int current_row = 0;

    for (const auto& vector : vectors) {
        result.block(current_row, 0, vector.rows(), 1) = vector;
        current_row += vector.rows();
    }

    return result;
}

void phgasnets::writeColumnsToCSV(
    const std::string& filename,
    const std::vector<std::string>& column_names,
    const std::vector<std::vector<double>>& columns
) {
    if (columns.empty()) return;
    // Check if column names and columns match
    size_t num_column_names = column_names.size();
    size_t num_columns = columns.size();
    if (num_column_names != num_columns) {
        std::cerr << "Error: Column names and columns mismatch.\n";
        return;
    }
    // Check if all columns have the same size
    size_t num_rows = columns[0].size();
    for (const auto& col : columns) {
        if (col.size() != num_rows) {
            std::cerr << "Error: Column size mismatch.\n";
            return;
        }
    }
    // Open File
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file.\n";
        return;
    }
    // Write column headers
    file << std::scientific << std::setprecision(6);
    for (size_t i = 0; i < column_names.size(); ++i) {
        file << column_names[i];
        if (i < column_names.size() - 1)
          file << " ";
    }
    file << "\n";

    for (size_t i = 0; i < num_rows; ++i) {
      for (size_t j = 0; j < num_columns; ++j) {
          file << columns[j][i];
          if (j < num_columns - 1)
              file << " ";
      }
      file << "\n";
    }

    file.close();
}
