#/**
 * @file gram_schmidt_mixed.cc
 * @brief Mixed-precision Gram-Schmidt example driver focused on plotting error differences between precisions.
 *
 * @details Runs classical and modified Gram-Schmidt (CGS/MGS) and the corresponding re-orthogonalisation (CGS2/MGS2) 
 * in mixed precisions for a variety of floating-point types. 
 * 
 * For this two different versions are possible:
 *  - using four different precisions for different operations
 *  - using two different precisions for the two iterations of CGS/MGS in CGS2/MGS2
 *    (for the CGS/MGS plots the second precision is used for all operations (no mixed precision))
 * 
 * Computes orthogonality and representation errors.
 * 
 * Orders and saves results to file in ./gnuplot/ and uses gnuplot to print.
 */
#include "../../hdnum.hh"
#include "../../src/gram_schmidt_mixed.hh"
#include "../../src/testmatrices.hh"

#include <array>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <tuple>
#include <string>
#include <vector>
#include <filesystem>
#include <ctime>
#include <set>

using namespace hdnum;


static std::filesystem::path matrix_dir = "./matrix/";  // default path to test matrices, can be overridden by command line argument
static std::filesystem::path data_dir = "gnuplot/data";
static std::filesystem::path plot_dir = "./gnuplot";
static std::size_t n = 100;  // default matrix rows, can be overridden by command line argument
static std::size_t m = 10;  // default matrix columns, can be overridden by command line argument


// #ifdef HDNUM_HAS_CPFLOAT
// #ifdef HDNUM_HAS_GMP

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

using ComboKey = std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>;
using ComboKey2 = std::tuple<std::size_t, std::size_t>;

/**
 * @brief ResultMap is used to store the results of four precision runs sorted by precision combination and kappa
 */
using ResultMap = std::map<ComboKey, std::map<double, Result>>;

/**
 * @brief ResultMap2 is used to store the results of two precision runs sorted by precision combination and kappa
 */
using ResultMap2 = std::map<ComboKey2, std::map<double, Result>>;
/**
 * @brief Tuple of all precision types that can be used.
 *
 * @details The test iterates over elements of this tuple to instantiate
 * mixed-precision variants of the Gram-Schmidt routines.
 */
using PrecisionTuple = std::tuple<FP8, bfloat16, FP16, FP32, FP64, FP128, FP256, FP512, FP1024>;

static constexpr std::array<const char*, 9> precision_names = {{
    "FP8", "bfloat16", "FP16", "FP32", "FP64", "FP128", "FP256", "FP512", "FP1024"
}};

/**
 * @brief Shorthand to convert indices to precision types i.e. Precision<4> => FP64
 */
template <std::size_t Index>
using Precision = std::tuple_element_t<Index, PrecisionTuple>;

/**
 * @brief Stores results of four precision runs before they are reordered into the plot data and saved to file later
 */
static ResultMap results;

/**
 * @brief Stores results of two precision runs before they are reordered into the plot data and saved to file later
 */
static ResultMap2 results2;

/**
 * @brief seed_str is first read in from the matrix file name and then stored globally to be later used in the plot file name
 */
static std::string seed_str;

/**
 * @brief Stores PrecisionTuple indices of unit roundoffs to be added to the plots.
 */
static std::vector<std::size_t> roundoff_indices;

/**
 * @brief Create a human-readable label for a precision-combination tuple.
 * @param combo Tuple containing the matrix, inner-product, normalization, and accumulation precision indices.
 * @return String describing the selected precision combination.
 */
std::string prec_label(const ComboKey& combo) {
    return precision_names[std::get<0>(combo)] + std::string("/") +
           precision_names[std::get<1>(combo)] + std::string("/") +
           precision_names[std::get<2>(combo)] + std::string("/") +
           precision_names[std::get<3>(combo)];
}

// helper to create a string label for a given combination of precisions
std::string prec_label(const ComboKey2& combo) {
    return precision_names[std::get<0>(combo)] + std::string("/") +
           precision_names[std::get<1>(combo)];
}

// helper to create a string label for a given combo of precisions
std::string prec_label1(const ComboKey2& combo) {
    return precision_names[std::get<1>(combo)];
}

/**
 * @brief Run one combination of four precisions and save errors as result.
 *
 * @details Instantiates and runs the mixed-precision classical, modified and re-orthogonalised
 * Gram-Schmidt algorithms on all matrices from MatrixMarket files in the given directory that match the naming scheme.
 * The indeces given as template parameters correspond to precisions in PrecisionTuple.
 *
 * @tparam I1 Index for `PrecisionTuple` specifying the matrix type.
 * @tparam I2 Index for `PrecisionTuple` specifying the inner-product type.
 * @tparam I3 Index for `PrecisionTuple` specifying the normalization type.
 * @tparam I4 Index for `PrecisionTuple` specifying the accumulation type.
 */
template <std::size_t I1, std::size_t I2, std::size_t I3, std::size_t I4>
void run_precisions() {
    using T1 = Precision<I1>;
    using T2 = Precision<I2>;
    using T3 = Precision<I3>;
    using T4 = Precision<I4>;

    for (const auto& dirEntry : recursive_directory_iterator(matrix_dir)) {
        std::string filename = dirEntry.path().filename().string();
        
        if (!(dirEntry.is_regular_file()) || !(dirEntry.path().extension() == ".mtx")
            || !(filename.find("matrix_") == 0)) {
            continue;
        }
        // parse filename to extract kappa
        std::size_t kappa_pos = filename.find("kappa_");
        std::size_t kappa_end = filename.find("_seed");
        if (kappa_pos == std::string::npos || kappa_end == std::string::npos || kappa_end <= kappa_pos + 6) {
            continue;
        }
        std::string kappa_str = filename.substr(kappa_pos + 6, kappa_end - (kappa_pos + 6));
        double kappa = std::stod(kappa_str);
        //parse filename for seed
        std::size_t seed_pos = filename.find("seed_u_v_");
        std::size_t seed_end = filename.find(".mtx");
        if (seed_pos == std::string::npos || seed_end == std::string::npos || seed_end <= seed_pos + 9) {
            continue;
        }
        // seed_str has form "seed_u_v_" + seed_u + "_" + seed_v
        // this is saved in plot filename
        seed_str = filename.substr(seed_pos, seed_end - (seed_pos));

        // read matrix from file
        DenseMatrix<T1> A(n, m);
        
        // for each matrix run_gram_schmidt
        readMatrixFromFileMatrixMarketHighPrec(A, dirEntry.path().string());
        // run test and save results
        Result res = run_gram_schmidt_mixed<T1, T2, T3, T4>(A, prec_label(std::make_tuple(I1,I2,I3,I4)), kappa_str);
        results[std::make_tuple(I1, I2, I3, I4)][kappa] = res;
    }

}

/**
 * @brief Run one combination of two precisions and save errors as result.
 *
 * @details Instantiates and runs the mixed-precision classical and modified
 * Gram-Schmidt algorithms in uniform precision 'I1'. CGS2 and MGS2 are performed 
 * with 'I1' in the first orthogonalisation and 'I2' for re-orthogonalisation.
 *
 * @tparam I1 Index for `PrecisionTuple` specifying the input matrix type.
 * @tparam I2 Index for `PrecisionTuple` specifying the output matrix type.
 * @param n Dimension of the test matrix.
 * @param m Dimension of the test matrix.
 */
template <std::size_t I1, std::size_t I2>
void run_precisions() {
    using T1 = Precision<I1>;
    using T2 = Precision<I2>;

    for (const auto& dirEntry : recursive_directory_iterator(matrix_dir)) {
        std::string filename = dirEntry.path().filename().string();
        // find matrices matching the naming pattern
        if (!(dirEntry.is_regular_file()) || !(dirEntry.path().extension() == ".mtx")
            || !(filename.find("matrix_") == 0)) {
            continue;
        }
        // parse filename to extract kappa
        std::size_t kappa_pos = filename.find("kappa_");
        std::size_t kappa_end = filename.find("_seed");
        if (kappa_pos == std::string::npos || kappa_end == std::string::npos || kappa_end <= kappa_pos + 6) {
            continue;
        }
        std::string kappa_str = filename.substr(kappa_pos + 6, kappa_end - (kappa_pos + 6));
        double kappa = std::stod(kappa_str);
        //parse filename for seed
        std::size_t seed_pos = filename.find("seed_u_v_");
        std::size_t seed_end = filename.find(".mtx");
        if (seed_pos == std::string::npos || seed_end == std::string::npos || seed_end <= seed_pos + 9) {
            continue;
        }
        // seed_str has form "seed_u_v_" + seed_u + "_" + seed_v
        // this is saved in plot filename
        seed_str = filename.substr(seed_pos, seed_end - (seed_pos));
        // read matrix from file
        DenseMatrix<T2> A(n, m);
        // for each matrix run_gram_schmidt
        readMatrixFromFileMatrixMarketHighPrec(A, dirEntry.path().string());
        // run test and save results
        Result res = run_gram_schmidt_mixed2<T1, T2>(A, prec_label(std::make_tuple(I1,I2)), kappa_str);
        results2[std::make_tuple(I1, I2)][kappa] = res;
        
    }

}

// ================================================================================

#pragma region plotting
// store and plot results using gnuplot scripts

/**
 * @brief Convert a precision-combination label into a filename-safe slug.
 * @param label Precision-combination label to encode.
 * @return Slug suitable for use in generated plot-data filenames.
 */
std::string label_slug(const ComboKey& combo) {
    std::string label = prec_label(combo);
    std::replace(label.begin(), label.end(), '/', '_');
    return label;
}

std::string label_slug(const ComboKey2& combo) {
    std::string label = prec_label(combo);
    std::replace(label.begin(), label.end(), '/', '_');
    return label;
}

/**
 * @brief Return the unit roundoff corresponding to one precision index.
 * @param precision_index Index for PrecisionTuple.
 * @return Unit roundoff as a double.
 */
double unit_roundoff_for_precision(std::size_t precision_index) {
    switch (precision_index) {
        case 0: return std::ldexp(1.0, -4);   // FP8
        case 1: return std::ldexp(1.0, -8);   // bfloat16
        case 2: return std::ldexp(1.0, -11);  // FP16
        case 3: return std::ldexp(1.0, -24);  // FP32
        case 4: return std::ldexp(1.0, -53);  // FP64
        case 5: return std::ldexp(1.0, -64);  // FP128
        case 6: return std::ldexp(1.0, -192); // FP256
        case 7: return std::ldexp(1.0, -448); // FP512
        case 8: return std::ldexp(1.0, -960); // FP1024
        default: return std::ldexp(1.0, -53);
    }
}

/**
 * @brief Describe one plotted series for the generated GNUplot output.
 */
struct PlotSeries {
    std::string title;
    std::filesystem::path data_file;
    std::vector<std::size_t> roundoff_indices;
};

/**
 * @brief Emit one plotted precision series as a solid linespoints trace plus dashed unit-roundoff overlays.
 * @param script Output stream for the generated GNUplot script.
 * @param series Series descriptor.
 * @param data_dir Directory holding the metric data files.
 * @param style_index Stable 1-based line type used for the baseline series.
 */
void write_precision_series(std::ofstream& script,
                const PlotSeries& series,
                const std::filesystem::path& data_dir,
                std::size_t style_index) {
    const std::string data_file = '"' + (data_dir / series.data_file.filename()).string() + '"';
    script << data_file
        << " using 1:2 with linespoints lw 2 lt " << style_index
        << " title '" << series.title << "'";

    for (std::size_t i = 0; i < series.roundoff_indices.size(); i++) {
        std::size_t precision_index = series.roundoff_indices[i];
        const std::string roundoff = (std::ostringstream() << std::scientific << std::setprecision(17)
            << unit_roundoff_for_precision(precision_index)).str();
        script << ", " << data_file
            << " using 1:(" << roundoff << ") with lines lw 2 lt " << (i+1)
            << " dt 2 notitle";
    }
}

/**
 * @brief Write a GNUplot plot command across multiple lines.
 * @tparam Writer Callable that emits one plot item for the given index.
 * @param script Output stream for the generated GNUplot script.
 * @param item_count Number of plot items to emit.
 * @param write_item Callable that writes one complete plot item.
 */
template <typename Writer>
void write_multiline_plot(std::ofstream& script, std::size_t item_count, Writer write_item) {
    script << "plot \\\n";
    for (std::size_t i = 0; i < item_count; ++i) {
        script << "    ";
        write_item(i);
        if (i + 1 < item_count) {
            script << ", \\\n";
        } else {
            script << "\n";
        }
    }
}

/**
 * @brief Write one metric to disk for each precision combination.
 * @tparam ValueGetter Callable returning the metric value from a Result object.
 * @param all_results Collected experiment results indexed by precision combination and condition number.
 * @param getter Callable extracting the requested metric from a Result.
 * @param metric_slug Base name for the generated data file.
 * @param data_dir Directory where the metric files are written.
 * @return Vector of plot-series descriptors for the generated files.
 */
template <typename ValueGetter>
std::vector<PlotSeries> write_metric_data(const ResultMap& all_results,
                                          ValueGetter getter,
                                          const std::string& metric_slug,
                                          const std::filesystem::path& data_dir) {
    std::vector<PlotSeries> series;

    for (const auto& result_it : all_results) {
        if ( result_it.second.empty()) {
            continue;
        }
        auto label = result_it.first;

        std::filesystem::path data_file = data_dir / (metric_slug + "_" + label_slug(label) + ".dat");
        std::ofstream out(data_file);
        if (!out) {
            std::cerr << "Failed to open plot data file: " << data_file << std::endl;
            continue;
        }

        out << std::scientific << std::setprecision(300);
        for (const auto& [kappa, res] : result_it.second) {
            out << kappa << ' ' << getter(res) << '\n';
        }

        series.push_back({prec_label(label), data_file, roundoff_indices});
    }

    return series;
}

/**
 * @brief Write one metric to disk for each precision.
 * @details This helper is only used in print_results2() to write the data for CGS and MGS.
 * Since CGS and MGS only use the second precision, only that is used for the legend precision labels.
 * Also if for example FP32/FP64 and FP64/64 are compared we only plot CGS/MGS in FP64 once.
 * @tparam ValueGetter Callable returning the metric value from a Result object.
 * @param all_results Collected experiment results indexed by precision combination and condition number.
 * @param getter Callable extracting the requested metric from a Result.
 * @param metric_slug Base name for the generated data file.
 * @param data_dir Directory where the metric files are written.
 * @return Vector of plot-series descriptors for the generated files.
 */
template <typename ValueGetter>
std::vector<PlotSeries> write_metric_data1(const ResultMap2& all_results,
                                          ValueGetter getter,
                                          const std::string& metric_slug,
                                          const std::filesystem::path& data_dir) {
    std::vector<PlotSeries> series;
    bool precisions[9] = {0,0,0,0,0,0,0,0,0};

    for (const auto& result_it : all_results) {
        if ( result_it.second.empty()) {
            continue;
        }
        // skip precisions we already have in the plot
        // (this only happens for CGS/MGS during the two precision run in some cases like: FP32/FP64 vs FP64/FP64)
        if (precisions[std::get<1>(result_it.first)] == true) {
            continue;
        } else {
            precisions[std::get<1>(result_it.first)] = true;
        }
        auto label = result_it.first;

        std::filesystem::path data_file = data_dir / (metric_slug + "_" + label_slug(label) + ".dat");
        std::ofstream out(data_file);
        if (!out) {
            std::cerr << "Failed to open plot data file: " << data_file << std::endl;
            continue;
        }

        out << std::scientific << std::setprecision(300);
        for (const auto& [kappa, res] : result_it.second) {
            out << kappa << ' ' << getter(res) << '\n';
        }

        series.push_back({prec_label1(label), data_file, roundoff_indices});
    }

    return series;
}

/**
 * @brief Write one metric to disk for each precision combination (uesd for two precision CGS2/MGS2).
 * @tparam ValueGetter Callable returning the metric value from a Result object.
 * @param all_results Collected experiment results indexed by precision combination and condition number.
 * @param getter Callable extracting the requested metric from a Result.
 * @param metric_slug Base name for the generated data file.
 * @param data_dir Directory where the metric files are written.
 * @return Vector of plot-series descriptors for the generated files.
 */
template <typename ValueGetter>
std::vector<PlotSeries> write_metric_data2(const ResultMap2& all_results,
                                          ValueGetter getter,
                                          const std::string& metric_slug,
                                          const std::filesystem::path& data_dir) {
    std::vector<PlotSeries> series;

    for (const auto& result_it : all_results) {
        if (result_it.second.empty()) {
            continue;
        }
        auto label = result_it.first;

        std::filesystem::path data_file = data_dir / (metric_slug + "_" + label_slug(label) + ".dat");
        std::ofstream out(data_file);
        if (!out) {
            std::cerr << "Failed to open plot data file: " << data_file << std::endl;
            continue;
        }

        out << std::scientific << std::setprecision(300);
        for (const auto& [kappa, res] : result_it.second) {
            out << kappa << ' ' << getter(res) << '\n';
        }

        series.push_back({prec_label(label), data_file, roundoff_indices});
    }

    return series;
}

/**
 * @brief Add one GNUplot panel for a metric plot to the plot script.
 * @param script Output stream for the generated GNUplot script.
 * @param title Panel title shown in the figure.
 * @param y_label Label for the y-axis.
 * @param series Plot series to include in the panel.
 * @param data_dir Directory holding the metric data files.
 * @param origin_x Horizontal origin of the panel.
 * @param origin_y Vertical origin of the panel.
 * @param width Panel width in normalized plot coordinates.
 * @param height Panel height in normalized plot coordinates.
 */
void write_plot_panel(std::ofstream& script,
                      const std::string& title,
                      const std::string& y_label,
                      const std::vector<PlotSeries>& series,
                      const std::filesystem::path& data_dir,
                      double origin_x,
                      double origin_y,
                      double width,
                      double height) {
    script << "set origin " << origin_x << "," << origin_y << "\n";
    script << "set size " << width << "," << height << "\n";
    script << "set title '" << title << "'\n";
    script << "set xlabel 'kappa'\n";
    script << "set ylabel '" << y_label << "'\n";
    script << "set logscale x\n";
    script << "set logscale y\n";
    script << "set grid\n";
    script << "set border\n";
    script << "set tics\n";
    script << "unset key\n";
    script << "unset xrange\n";
    script << "unset yrange\n";

    if (series.empty()) {
        script << "plot NaN notitle\n";
        return;
    }

    write_multiline_plot(script, series.size(), [&](std::size_t i) {
        write_precision_series(script, series[i], data_dir, i + 1);
    });
    script << "unset logscale x\n";
    script << "unset logscale y\n";
}

/**
 * @brief Add a legend panel to the generated GNUplot script.
 * @param script Output stream for the generated GNUplot script.
 * @param series Plot series for which the legend is generated.
 * @param origin_x Horizontal origin of the legend panel.
 * @param origin_y Vertical origin of the legend panel.
 * @param width Panel width in normalized plot coordinates.
 * @param height Panel height in normalized plot coordinates.
 */
void write_legend_panel(std::ofstream& script,
                        const std::vector<PlotSeries>& series,
                        double origin_x,
                        double origin_y,
                        double width,
                        double height) {
    script << "set origin " << origin_x << "," << origin_y << "\n";
    script << "set size " << width << "," << height << "\n";
    script << "unset border\n";
    script << "unset tics\n";
    script << "unset title\n";
    script << "unset xlabel\n";
    script << "unset ylabel\n";
    script << "unset logscale x\n";
    script << "unset logscale y\n";
    script << "set xrange [0:1]\n";
    script << "set yrange [0:1]\n";
    script << "set key horizontal center top box opaque\n";
    script << "set key samplen 1.2\n";
    script << "set key spacing 1.1\n";

    if (series.empty()) {
        script << "plot NaN notitle\n";
        return;
    }

    write_multiline_plot(script, series.size(), [&](std::size_t i) {
        script << "2 with linespoints lw 2 lt " << (i + 1)
               << " title '" << series[i].title << "'";
    });
}

/**
 * @brief Add a legend panel explaining the dashed unit-roundoff references.
 * @param script Output stream for the generated GNUplot script.
 * @param roundoff_indices Precision indices to include in the legend.
 * @param origin_x Horizontal origin of the legend panel.
 * @param origin_y Vertical origin of the legend panel.
 * @param width Panel width in normalized plot coordinates.
 * @param height Panel height in normalized plot coordinates.
 */
void write_roundoff_legend_panel(std::ofstream& script,
                                 const std::vector<std::size_t>& roundoff_indices,
                                 double origin_x,
                                 double origin_y,
                                 double width,
                                 double height) {
    script << "set origin " << origin_x << "," << origin_y << "\n";
    script << "set size " << width << "," << height << "\n";
    script << "unset border\n";
    script << "unset tics\n";
    script << "unset title\n";
    script << "unset xlabel\n";
    script << "unset ylabel\n";
    script << "unset logscale x\n";
    script << "unset logscale y\n";
    script << "set xrange [0:1]\n";
    script << "set yrange [0:1]\n";
    script << "set key horizontal center top box opaque\n";
    script << "set key samplen 1.2\n";
    script << "set key spacing 1.1\n";

    if (roundoff_indices.empty()) {
        script << "plot NaN notitle\n";
        return;
    }

    write_multiline_plot(script, roundoff_indices.size(), [&](std::size_t i) {
        const std::size_t precision_index = roundoff_indices[i];
        script << "2 with lines lw 2 lt " << (i+1)
               << " dt 2 title '" << precision_names[precision_index]
               << " unit roundoff'";
    });
}

/**
 * @brief Add a combined legend panel for precision combination labels and roundoff references.
 * @param script Output stream for the generated GNUplot script.
 * @param algorithm_series Algorithm series to include.
 * @param roundoff_indices Precision indices to include in the roundoff legend.
 * @param origin_x Horizontal origin of the legend panel.
 * @param origin_y Vertical origin of the legend panel.
 * @param width Panel width in normalized plot coordinates.
 * @param height Panel height in normalized plot coordinates.
 */
void write_combined_legend_panel(std::ofstream& script,
                                 const std::vector<PlotSeries>& algorithm_series,
                                 const std::vector<std::size_t>& roundoff_indices,
                                 double origin_x,
                                 double origin_y,
                                 double width,
                                 double height) {
    script << "set origin " << origin_x << "," << origin_y << "\n";
    script << "set size " << width << "," << height << "\n";
    script << "unset border\n";
    script << "unset tics\n";
    script << "unset title\n";
    script << "unset xlabel\n";
    script << "unset ylabel\n";
    script << "unset logscale x\n";
    script << "unset logscale y\n";
    script << "set xrange [0:1]\n";
    script << "set yrange [0:1]\n";
    script << "set key horizontal center top box opaque\n";
    script << "set key samplen 1.2\n";
    script << "set key spacing 1.1\n";

    if (algorithm_series.empty() && roundoff_indices.empty()) {
        script << "plot NaN notitle\n";
        return;
    }

    const std::size_t total_items = algorithm_series.size() + roundoff_indices.size();
    write_multiline_plot(script, total_items, [&](std::size_t i) {
        if (i < algorithm_series.size()) {
            script << "2 with linespoints lw 2 lt " << (i + 1)
                   << " title '" << algorithm_series[i].title << "'";
        } else {
            const std::size_t roundoff_offset = i - algorithm_series.size();
            const std::size_t precision_index = roundoff_indices[roundoff_offset];
            script << "2 with lines lw 2 lt " << (roundoff_offset + 1)
                   << " dt 2 title '" << precision_names[precision_index]
                   << " unit roundoff'";
        }
    });
}

/**
 * @brief Generate a multi-panel GNUplot figure for the CGS/MGSCGS2/MGS2 four precision error metrics.
 * @param all_results Results indexed by precision combination label and condition number.
 */void print_results(const ResultMap& all_results, const std::string& title) {
    std::filesystem::create_directories(data_dir);

    // write metric data to file
    const auto cgs_ortho = write_metric_data(
        all_results,
        [](const Result& res) { return res.ortho_err_cgs; },
        "cgs_orthogonality_error",
        data_dir);

    const auto mgs_ortho = write_metric_data(
        all_results,
        [](const Result& res) { return res.ortho_err_mgs; },
        "mgs_orthogonality_error",
        data_dir);
    
    const auto cgs2_ortho = write_metric_data(
        all_results,
        [](const Result& res) { return res.ortho_err_cgs2; },
        "cgs2_orthogonality_error",
        data_dir);

    const auto mgs2_ortho = write_metric_data(
        all_results,
        [](const Result& res) { return res.ortho_err_mgs2; },
        "mgs2_orthogonality_error",
        data_dir);

    const auto cgs_rep = write_metric_data(
        all_results,
        [](const Result& res) { return res.rep_err_cgs; },
        "cgs_representation_error",
        data_dir);
    
    const auto cgs2_rep = write_metric_data(
        all_results,
        [](const Result& res) { return res.rep_err_cgs2; },
        "cgs2_representation_error",
        data_dir);

    const auto mgs_rep = write_metric_data(
        all_results,
        [](const Result& res) { return res.rep_err_mgs; },
        "mgs_representation_error",
        data_dir);

    const auto mgs2_rep = write_metric_data(
        all_results,
        [](const Result& res) { return res.rep_err_mgs2; },
        "mgs2_representation_error",
        data_dir);

    // add seed and timestamp to filename
    auto t = std::time(NULL);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M");
    std::string timestamp = oss.str();
    std ::string plot_name = "mgs_plot_" + seed_str + "_" + timestamp + ".png";
    const std::filesystem::path script_file = "gram_schmidt_mixed_plots.gp";
    const std::filesystem::path plot_file = plot_dir / plot_name;

    std::ofstream script(script_file);
    if (!script) {
        std::cerr << "Failed to open gnuplot script file: " << script_file << std::endl;
        return;
    }

        script << "set terminal pngcairo size 3000,1200 enhanced font ',16'\n";
    script << "set output '" << plot_file.string() << "'\n";
    script << "set multiplot title '" << title << "'\n";

    // set position and size of each panel in the multiplot layout
    double space = 0.0;
    double legend_width = 0.99;
    double legend_height = 0.06;
    double plot_width = (1 - 3*space) / 4;
    double plot_height = (1 - legend_height - 2*space) / 2;
    

    write_combined_legend_panel(script,
                                cgs_ortho,
                                roundoff_indices,
                                0.00,
                                (2*plot_height + space),
                                legend_width,
                                legend_height);
    write_plot_panel(script,
                     "CGS loss of orthogonality vs kappa",
                     "orthogonality error",
                     cgs_ortho,
                     data_dir,
                                         0.00,
                                         (plot_height + space),
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "MGS loss of orthogonality vs kappa",
                     "orthogonality error",
                     mgs_ortho,
                     data_dir,
                                         (plot_width + space),
                                         (plot_height + space),
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "CGS2 loss of orthogonality vs kappa",
                     "orthogonality error",
                     cgs2_ortho,
                     data_dir,
                                         (2*plot_width + 2*space),
                                         (plot_height + space),
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "MGS2 loss of orthogonality vs kappa",
                     "orthogonality error",
                     mgs2_ortho,
                     data_dir,
                                         (3*plot_width + 3*space),
                                         (plot_height + space),
                                         plot_width,
                                         plot_height);                                     
    write_plot_panel(script,
                     "CGS representation error vs kappa",
                     "relative representation error",
                     cgs_rep,
                     data_dir,
                                         0.00,
                                         0.00,
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "MGS representation error vs kappa",
                     "relative representation error",
                     mgs_rep,
                     data_dir,
                                         (plot_width + space),
                                         0.00,
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "CGS2 representation error vs kappa",
                     "relative representation error",
                     cgs2_rep,
                     data_dir,
                                         (2*plot_width + 2*space),
                                         0.00,
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "MGS2 representation error vs kappa",
                     "relative representation error",
                     mgs2_rep,
                     data_dir,
                                         (3*plot_width + 3*space),
                                         0.00,
                                         plot_width,
                                         plot_height);
    script << "unset multiplot\n";
    script << "set output\n";
    script.close();

    const std::string command = "gnuplot " + script_file.string();
    const int exit_code = std::system(command.c_str());
    if (exit_code == 0) {
        std::cout << "Wrote plot to " << plot_file << std::endl;
    } else {
        std::cout << "Wrote gnuplot script to " << script_file << std::endl;
        std::cout << "Run manually with: gnuplot " << script_file << std::endl;
    }
}


// helper to plot a given metric for selected precision combinations against kappa
/**
 * @brief Generate a multi-panel GNUplot figure comparing uniform precision CGS/MGS to varying precision iterations CGS2/MGS2.
 * 
 * @details Errors that were calculated previously in run_precisions() and stored in Results are written to file for gnuplot to read in.
 * Spacing between panels is set and gnuplot is called. 
 * 
 * @param all_results Results indexed by precision combination and condition number.
 */
void print_results2(const ResultMap2& all_results, const std::string& title) {
    std::filesystem::create_directories(data_dir);

    const auto cgs_ortho = write_metric_data1(
        all_results,
        [](const Result& res) { return res.ortho_err_cgs; },
        "cgs_orthogonality_error",
        data_dir);

    const auto mgs_ortho = write_metric_data1(
        all_results,
        [](const Result& res) { return res.ortho_err_mgs; },
        "mgs_orthogonality_error",
        data_dir);
    
    const auto cgs2_ortho = write_metric_data2(
        all_results,
        [](const Result& res) { return res.ortho_err_cgs2; },
        "cgs2_orthogonality_error",
        data_dir);

    const auto mgs2_ortho = write_metric_data2(
        all_results,
        [](const Result& res) { return res.ortho_err_mgs2; },
        "mgs2_orthogonality_error",
        data_dir);

    const auto cgs_rep = write_metric_data1(
        all_results,
        [](const Result& res) { return res.rep_err_cgs; },
        "cgs_representation_error",
        data_dir);
    
    const auto cgs2_rep = write_metric_data2(
        all_results,
        [](const Result& res) { return res.rep_err_cgs2; },
        "cgs2_representation_error",
        data_dir);

    const auto mgs_rep = write_metric_data1(
        all_results,
        [](const Result& res) { return res.rep_err_mgs; },
        "mgs_representation_error",
        data_dir);

    const auto mgs2_rep = write_metric_data2(
        all_results,
        [](const Result& res) { return res.rep_err_mgs2; },
        "mgs2_representation_error",
        data_dir);

    // add seed and timestamp to filename
    auto t = std::time(NULL);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d_%H-%M");
    std::string timestamp = oss.str();
    std ::string plot_name = "mgs_plot_" + seed_str + "_" + timestamp + ".png";
    const std::filesystem::path script_file = "gram_schmidt_mixed_plots.gp";
    const std::filesystem::path plot_file = plot_dir / plot_name;

    std::ofstream script(script_file);
    if (!script) {
        std::cerr << "Failed to open gnuplot script file: " << script_file << std::endl;
        return;
    }

        script << "set terminal pngcairo size 3000,1200 enhanced font ',16'\n";
    script << "set output '" << plot_file.string() << "'\n";
    script << "set multiplot title '" << title << "'\n";

    // set position and size of each panel in the multiplot layout
    double space = 0.0;
    double legend_width = 0.33;
    double legend_height = 0.06;
    double plot_width = (1 - 3*space) / 4;
    double plot_height = (1 - legend_height - 2*space) / 2;
    

    write_legend_panel(script,
                                            cgs_ortho,
                                            0.00,
                                            (2*plot_height + space),
                                            legend_width,
                                            legend_height);
    std::vector<PlotSeries> roundoff_legend_series = cgs_ortho;
    roundoff_legend_series.insert(roundoff_legend_series.end(), mgs_ortho.begin(), mgs_ortho.end());
    roundoff_legend_series.insert(roundoff_legend_series.end(), cgs2_ortho.begin(), cgs2_ortho.end());
    roundoff_legend_series.insert(roundoff_legend_series.end(), mgs2_ortho.begin(), mgs2_ortho.end());
    write_roundoff_legend_panel(script,
                                            roundoff_indices,
                                            0.33,
                                            (2*plot_height + space),
                                            legend_width,
                                            legend_height);
    write_legend_panel(script,
                                            cgs2_ortho,
                                            0.66,
                                            (2*plot_height + space),
                                            legend_width,
                                            legend_height);
    write_plot_panel(script,
                     "CGS loss of orthogonality vs kappa",
                     "orthogonality error",
                     cgs_ortho,
                     data_dir,
                                         0.00,
                                         (plot_height + space),
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "MGS loss of orthogonality vs kappa",
                     "orthogonality error",
                     mgs_ortho,
                     data_dir,
                                         (plot_width + space),
                                         (plot_height + space),
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "CGS2 loss of orthogonality vs kappa",
                     "orthogonality error",
                     cgs2_ortho,
                     data_dir,
                                         (2*plot_width + 2*space),
                                         (plot_height + space),
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "MGS2 loss of orthogonality vs kappa",
                     "orthogonality error",
                     mgs2_ortho,
                     data_dir,
                                         (3*plot_width + 3*space),
                                         (plot_height + space),
                                         plot_width,
                                         plot_height);                                     
    write_plot_panel(script,
                     "CGS representation error vs kappa",
                     "relative representation error",
                     cgs_rep,
                     data_dir,
                                         0.00,
                                         0.00,
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "MGS representation error vs kappa",
                     "relative representation error",
                     mgs_rep,
                     data_dir,
                                         (plot_width + space),
                                         0.00,
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "CGS2 representation error vs kappa",
                     "relative representation error",
                     cgs2_rep,
                     data_dir,
                                         (2*plot_width + 2*space),
                                         0.00,
                                         plot_width,
                                         plot_height);
    write_plot_panel(script,
                     "MGS2 representation error vs kappa",
                     "relative representation error",
                     mgs2_rep,
                     data_dir,
                                         (3*plot_width + 3*space),
                                         0.00,
                                         plot_width,
                                         plot_height);
    script << "unset multiplot\n";
    script << "set output\n";
    script.close();

    const std::string command = "gnuplot " + script_file.string();
    const int exit_code = std::system(command.c_str());
    if (exit_code == 0) {
        std::cout << "Wrote plot to " << plot_file << std::endl;
    } else {
        std::cout << "Wrote gnuplot script to " << script_file << std::endl;
        std::cout << "Run manually with: gnuplot " << script_file << std::endl;
    }
}

#pragma endregion plotting

// ================================================================================

#pragma region main

/**
 * @brief Entry point for the mixed-precision Gram-Schmidt test.
 *
 * @details Loads all matching matrices from a runtime directory, runs the selected
 * precision combinations, and plots the collected results against kappa.
 * The directory holding the matrices and matrix dimensions can be given as command line arguments
 * to use the same precision combination on different matrix selections.
 * 
 * Note: all precision combinations used in the final report are left in, but commented out.
 * Only one plot is generated at a time, so only one of these settings can be run at a time
 * 
 * Example (baseline):
 * @code
 * title = "Mixed precision Gram-Schmidt baseline over low precision to double precision";
 * roundoff_indices = {0,1,2,3,4};
 * run_precisions<0,0,0,0>(n, m);
 * run_precisions<1,1,1,1>(n, m);
 * run_precisions<2,2,2,2>(n, m);
 * run_precisions<3,3,3,3>(n, m);
 * run_precisions<4,4,4,4>(n, m);
 * print_results(results, title);
 * @endcode
 * 
 * Example (varying iteration precision for re-orthogonalisation):
 * @code
 * title2 = "Standard uniform precision CGS/MGS vs first iteration in low precision second iteration high precision CGS2/MGS2";
 * roundoff_indices = {3,4};
 * run_precisions<1,4>(n,m);
 * run_precisions<3,4>(n,m);
 * run_precisions<4,4>(n,m);
 * run_precisions<3,3>(n,m);
 * print_results2(results2, title2);
 * @endcode
 * 
 * 
 *
 * @return Exit code (0 on success).
 */
int main(int argc, char* argv[]) {
    if (argc >= 2) {
        matrix_dir = argv[1];
    }
    if (argc >= 3) {
        n = std::stoul(argv[2]);
    }
    if (argc >= 4) {
        m = std::stoul(argv[3]);
    }

    if (!std::filesystem::exists(matrix_dir)) {
        std::cerr << "Input directory does not exist: " << matrix_dir << std::endl;
        return 1;
    }
    if (!std::filesystem::is_directory(matrix_dir)) {
        std::cerr << "Input path is not a directory: " << matrix_dir << std::endl;
        std::cerr << "Usage: ./mgs_run [matrix_directory] [n] [m]" << std::endl;
        return 1;
    }

    results.clear();

    // default titles
    std::string title = "Mixed precision Gram-Schmidt results";
    std::string title2 = "Standard uniform precision CGS/MGS vs first iteration in low precision second iteration high precision CGS2/MGS2";


    // Settings used in report:

    // run gram schmidt without mixed precision as a baseline (only to FP64)
    title = "Mixed precision Gram-Schmidt baseline FP8 to FP64";
    roundoff_indices = {0,1,2,3,4};
    run_precisions<0,0,0,0>();
    run_precisions<1,1,1,1>();
    run_precisions<2,2,2,2>();
    run_precisions<3,3,3,3>();
    run_precisions<4,4,4,4>();
    print_results(results, title);

    // run gram schmidt without mixed precision as a baseline (GMP)
    // this one was only mentioned in report
    // title = "Mixed precision Gram-Schmidt baseline for high precisions";
    // roundoff_indices = {5,6,7,8};
    // run_precisions<5,5,5,5>();
    // run_precisions<6,6,6,6>();
    // run_precisions<7,7,7,7>();
    // run_precisions<8,8,8,8>();
    // print_results(results, title);

    // test 'saving' on FP64
    // title = "Mixed precision Gram-Schmidt test using lower precision for one operation type";
    // roundoff_indices = {3,4};
    // run_precisions<4,4,4,4>();
    // run_precisions<4,3,4,4>();
    // run_precisions<4,4,3,4>();
    // run_precisions<4,4,4,3>();
    // run_precisions<4,3,3,3>();
    // print_results(results, title);

    // compare over FP32/FP64
    // title = "Mixed precision Gram-Schmidt test using higher precision for one operation type";
    // roundoff_indices = {3};
    // run_precisions<3,3,3,3>();
    // run_precisions<3,4,3,3>();
    // run_precisions<3,3,4,3>();
    // run_precisions<3,3,3,4>();
    // print_results(results, title);

    // compare over FP64/FP128
    // title = "Mixed precision Gram-Schmidt test using higher precision for one operation type";
    // roundoff_indices = {4};
    // run_precisions<4,4,4,4>();
    // run_precisions<4,5,4,4>();
    // run_precisions<4,4,5,4>();
    // run_precisions<4,4,4,5>();
    // print_results(results, title);

    // compare over FP64/FP1024
    // title = "Mixed precision Gram-Schmidt test using higher precision for one operation type";
    // this one was only mentioned in report
    // roundoff_indices = {4};
    // run_precisions<4,4,4,4>();
    // run_precisions<4,8,4,4>();
    // run_precisions<4,4,8,4>();
    // run_precisions<4,4,4,8>();
    // print_results(results, title);

    // fixed low projection + normalization, high inner product
    // title = "Mixed precision Gram-Schmidt: high precision inner products, low precison projections and normalisation";
    // roundoff_indices = {1,3};
    // run_precisions<4,8,1,1>();
    // run_precisions<4,8,3,3>();
    // run_precisions<4,1,1,1>();
    // run_precisions<4,3,3,3>();
    // print_results(results, title);

    // test CGS2.2
    // title2 = "Mixed precision Gram-Schmidt: re-orthogonalisation with higher precision";
    // roundoff_indices = {1,3,4};
    // run_precisions<1,4>();
    // run_precisions<3,4>();
    // run_precisions<4,4>();
    // run_precisions<3,3>();
    // print_results2(results2, title2);

    return 0;
}

#pragma endregion main

// #endif
// #endif