#pragma once


namespace part_sort {

    std::string erase_file_extension(std::string& input);

    std::string obtain_number(std::string input);

    std::string erase_zeroes(std::string input);


    int process_partition_number(std::string input);

    std::string process_partition_number_as_string(std::string input);

    bool compare_partition_values(std::string string_1, std::string string_2);

    void swap_string_vals(std::string* p1, std::string* p2);

    void bubble_sort(std::vector<std::string>& vec);

    void quick_sort_recursive(int32_t beg, int32_t end, std::vector<std::string>& vec);

    void quick_sort(std::vector<std::string>& vec);

};
