#include <iostream>

class Dynamo_array {
 public:
  int two_dim_arr_size{};
  int** two_dim_arr = new int* [0];

  void push_back_array(int size) {
    two_dim_arr_size++;
    int** new_two_dim_arr = new int* [two_dim_arr_size];
    memcpy(new_two_dim_arr, two_dim_arr, two_dim_arr_size - 1);
    new_two_dim_arr[two_dim_arr_size - 1] = new int[size + 1];
    new_two_dim_arr[two_dim_arr_size - 1][0] = size;
    for (int i = 0; i < two_dim_arr_size - 1; ++i) {
      delete[] two_dim_arr[i];
    }
    two_dim_arr = new_two_dim_arr;
  }
  void push_back_num(int row_index, int col_number, int number) const {
    two_dim_arr[row_index][col_number] = number;
  }
  void print() const {
    std::cout << "two dimensional array of size: " << two_dim_arr_size << "\n";
    for (int i = 0; i < two_dim_arr_size; ++i) {
      std::cout << "array with row " << i << " and size " << two_dim_arr[i][0] << " is {: ";
      for (int j = 1; j < two_dim_arr[i][0] + 1; ++j) {
        std::cout << two_dim_arr[i][j] << " ";
      }
      std::cout << ":}\n";
    }
    std::cout << "that's all\n\n";
  }
  void clear() const {
    for (int i = 0; i < two_dim_arr_size; ++i) {
      delete[] two_dim_arr[i];
    }
  }
  long long get_permutation_sum(int*& bust) const {
    long long sum = 1;
    for (int i = 0; i < two_dim_arr_size; ++i) {
      sum *= two_dim_arr[i][bust[i]];
    }
    return sum;
  }
  void print_permutation(int*& bust, int index) const {
    std::cout << "current permutation with index " << index << " is ";
    for (int i = 0; i < two_dim_arr_size; ++i) {
      std::cout << bust[i] - 1 << " ";
    }
    std::cout << "result sum is " << get_permutation_sum(bust) << "\n";
  }
  void permutate(int*& bust, long long& result_sum, int index) {
    if (bust[index] == two_dim_arr[index][0] + 1) {
      if (index != 0) {
        bust[index] = 1;
        bust[index - 1]++;
        index--;
        permutate(bust, result_sum, index);
      }
    } else {
      if (index == two_dim_arr_size - 1) {
        if (bust[index] != 0) result_sum += get_permutation_sum(bust);
        bust[index]++;
      } else if (index == 0) {
        if (bust[index] != two_dim_arr[index][0] + 1) {
          index++;
        }
      } else {
        index++;
      }
      permutate(bust, result_sum, index);
    }
  }

};

int main(int argc, char** argv) {
  Dynamo_array dynamic_array;
  int num;
  for (int i = 1; i < argc; ++i) {
    num = std::atoi(argv[i]);
    std::cout << num << '\n';
    dynamic_array.push_back_array(num);
  }
  int input_num = 0;
  for (int i = 0; i < dynamic_array.two_dim_arr_size; ++i) {
    for (int j = 1; j < dynamic_array.two_dim_arr[i][0] + 1; ++j) {
      std::cin >> input_num;
      dynamic_array.push_back_num(i, j, input_num);
    }
  }
  long long result_sum = 0;
  int* bust = new int[dynamic_array.two_dim_arr_size];
  for (int i = 0; i < dynamic_array.two_dim_arr_size; ++i) {
    bust[i] = 1;
  }
  dynamic_array.permutate(bust, result_sum, 0);
  std::cout << result_sum;
  delete[] bust;
  dynamic_array.clear();
  return 0;
}