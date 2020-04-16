#include "../../../source/tools/File.h"
#include "../../../source/tools/vector_utils.h"

int main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cout << "Must include a single filename for data." << std::endl;
    exit(1);
  }

  emp::File file(argv[1]);
  file.RemoveWhitespace();  // Clear out all whitespace in file.
  file.RemoveEmpty();       // Remove all now-empty lines from file.

  if (file.GetNumLines() == 0) {
    std::cout << "No data found. Exiting." << std::endl;
    exit(2);
  }

  std::cout << "Found data for " << file.GetNumLines() << " histograms." << std::endl;

  auto data = file.ToData<double>();

  // Analyze base data.
  double min_val = data[0][0];
  double max_val = min_val;
  double total_val = 0.0;
  size_t num_vals = 0;

  for (auto & row : data) {
    for (double val : row) {
      if (val < min_val) min_val = val;
      if (val > max_val) max_val = val;
      total_val += val;
      num_vals++;
    }
  }

  // Collect the full histogram.
  size_t bins = 40;
  double full_span = (max_val - min_val) * 1.00001;
  double bin_width = full_span / (double) bins;
  emp::vector<size_t> bin_counts(bins, 0);
  for (auto & row : data) {
    for (double val : row) {
      size_t cur_bin = (size_t) ((val - min_val) / bin_width);
      bin_counts[cur_bin]++;
    }
  }
  size_t max_bin_count = 0;
  for (size_t count : bin_counts) if (count > max_bin_count) max_bin_count = count;


  while (file.GetNumLines()) {
    emp::vector<double> row = file.ExtractRowAs<double>();
    emp::Sort(row);

    std::cout << "MIN_VAL: " << min_val << std::endl;
    double cap = min_val;
    double pos = 0;
    size_t count = 0;
    for (size_t bin_id = 0; bin_id < bins; bin_id++) {
      cap += bin_width;
      while (pos < row.size() && row[pos] <= cap) {
        pos++;
        std::cout << "*";
        count++;
      }
      std::cout << std::endl;
    }
    std::cout << "MAX_VAL: " << max_val << std::endl;
    std::cout << "Data count: " << count << std::endl;
  }

  std::cout << "OVERALL COUNT: " << num_vals << std::endl;
  std::cout << "OVERALL MIN:   " << min_val << std::endl;
  std::cout << "OVERALL MAX:   " << max_val << std::endl;
  std::cout << "OVERALL MEAN:  " << (total_val/(double) num_vals) << std::endl;

  double scale = ((double) max_bin_count) / 80.0;
  if (scale < 1.0) scale = 1.0;
  for (size_t count : bin_counts) {
    for (size_t i = 0; i < (count/scale); i++) std::cout << "*";
    std::cout << std::endl;
  }
}
