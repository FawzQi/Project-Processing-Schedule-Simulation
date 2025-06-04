#include <raylib.h>

#include <algorithm>
#include <deque>  // Untuk std::deque
#include <iostream>
#include <set>
#include <string>
#include <vector>

struct process_t {
    int id;
    int arrival_time;
    int burst_time;
    int remaining_time;
    Color color;
};

struct RR_process_t {
    int id;
    int burst_time;
};

enum class menu_t {
    HOME = 0,
    FCFS,
    SJF,
    SJFP,
    RR,
};

void get_input_keyboard(std::string& input_string) {
    int key = GetKeyPressed();

    if (key >= KEY_ZERO && key <= KEY_NINE) {
        input_string += (char)(key - KEY_ZERO + '0');
    } else if (key == KEY_BACKSPACE && !input_string.empty()) {
        input_string.pop_back();
    }
}

void get_input_number(int& variable) {
    int number = GetKeyPressed();
    if (number >= 48 && number <= 57) {
        variable = number - 48;
    }
}

std::vector<process_t> processes = {
    {0, 2, 10, 10, BLACK},
    {1, 6, 3, 3, RED},
    {2, 7, 7, 7, BLUE},
    {3, 1, 8, 8, GREEN},
    {4, 10, 14, 14, YELLOW},
    {5, 20, 20, 20, PURPLE},
    {6, 15, 30, 30, ORANGE},
    {7, 13, 5, 5, BROWN},

};

void add_process(int id, int arrival_time, int burst_time) {
    if (processes.size() < 8) {
        process_t new_process;
        new_process.id = id;
        new_process.arrival_time = arrival_time;
        new_process.burst_time = burst_time;
        new_process.remaining_time = burst_time;
        new_process.color = Color{(unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), 255};
        processes.push_back(new_process);
    }
}

void delete_process() {
    if (processes.empty()) return;
    processes.pop_back();
}

bool is_all_processes_finished() {
    for (const auto& process : processes) {
        if (process.remaining_time > 0) {
            return false;
        }
    }
    return true;
}

void sort_processes(menu_t algorithm) {
    if (algorithm == menu_t::FCFS) {
        sort(processes.begin(), processes.end(), [](const process_t& a, const process_t& b) {
            return a.arrival_time < b.arrival_time;
        });
    } else if (algorithm == menu_t::SJF) {
        sort(processes.begin(), processes.end(), [](const process_t& a, const process_t& b) {
            return a.burst_time < b.burst_time;
        });
    } else if (algorithm == menu_t::SJFP) {
        // Implement SJF-P logic here
    } else if (algorithm == menu_t::RR) {
        sort(processes.begin(), processes.end(), [](const process_t& a, const process_t& b) {
            return a.arrival_time < b.arrival_time;
        });
    }
}

int time_frame = 0;
int start_simulation = 0;
int process_count = 1;

std::vector<process_t> queued_processes;
std::vector<process_t> ongoing_processes;
std::vector<process_t> finished_processes;
process_t current_process;
float process_start_time = 0;
float process_end_time = 0;

int queue_iterator = 0;
int ongoing_process_iterator = 0;
int ongoing_process_runtime = 0;

menu_t display_menu = menu_t::HOME;
menu_t algorithm_type = menu_t::FCFS;
int get_input_process = 0;
bool get_input_arrival_time = false;
bool get_input_burst_time = false;

void display(menu_t menu) {
    if (menu == menu_t::HOME) {
        DrawText("Home", 10, 10, 20, BLACK);

        DrawText("Select Algorithm", 10, 40, 20, BLACK);
        Rectangle fcfs_button = {10, 80, 100, 25};
        if (algorithm_type == menu_t::FCFS) {
            DrawRectangleRec(fcfs_button, DARKGRAY);
        } else {
            DrawRectangleRec(fcfs_button, LIGHTGRAY);
        }
        DrawText("FCFS", fcfs_button.x + 5, fcfs_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), fcfs_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            algorithm_type = menu_t::FCFS;
        }
        Rectangle sjf_button = {10, 120, 100, 25};
        if (algorithm_type == menu_t::SJF) {
            DrawRectangleRec(sjf_button, DARKGRAY);
        } else {
            DrawRectangleRec(sjf_button, LIGHTGRAY);
        }
        DrawText("SJF", sjf_button.x + 5, sjf_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), sjf_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            algorithm_type = menu_t::SJF;
        }
        // Rectangle sjfp_button = {10, 160, 100, 25};
        // if (algorithm_type == menu_t::SJFP) {
        //     DrawRectangleRec(sjfp_button, DARKGRAY);
        // } else {
        //     DrawRectangleRec(sjfp_button, LIGHTGRAY);
        // }
        // DrawText("SJF-P", sjfp_button.x + 5, sjfp_button.y + 5, 20, BLACK);
        // if (CheckCollisionPointRec(GetMousePosition(), sjfp_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        //     algorithm_type = menu_t::SJFP;
        // }
        Rectangle rr_button = {10, 160, 100, 25};
        if (algorithm_type == menu_t::RR) {
            DrawRectangleRec(rr_button, DARKGRAY);
        } else {
            DrawRectangleRec(rr_button, LIGHTGRAY);
        }
        DrawText("RR", rr_button.x + 5, rr_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), rr_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            algorithm_type = menu_t::RR;
        }

        DrawText("Process List", 260, 10, 20, BLACK);

        Rectangle add_process_button = {450, 40, 20, 25};
        DrawRectangleRec(add_process_button, GRAY);
        DrawText("+  Add", add_process_button.x + 5, add_process_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), add_process_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            add_process(processes.size(), 0, 0);
        }

        static std::string input_number;
        Rectangle delete_process_button = {260.0f, 40.0f, 20.0f, 25.0f};
        DrawRectangleRec(delete_process_button, RED);
        DrawText("- Delete ", delete_process_button.x + 5, delete_process_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), delete_process_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            delete_process();
        }

        static bool is_clicked = false;

        for (int i = 0; i < processes.size(); ++i) {
            Rectangle arrival_time_box = {440.0f, 80.0f + i * 25.0f, 160.0f, 20.0f};
            if (!get_input_arrival_time)
                DrawRectangleRec(arrival_time_box, LIGHTGRAY);
            else {
                if (get_input_process == i + 1) {
                    DrawRectangleRec(arrival_time_box, GRAY);
                } else {
                    DrawRectangleRec(arrival_time_box, LIGHTGRAY);
                }
            }

            Rectangle burst_time_box = {620.0f, 80.0f + i * 25.0f, 140.0f, 20.0f};
            if (!get_input_burst_time)
                DrawRectangleRec(burst_time_box, LIGHTGRAY);
            else {
                if (get_input_process == i + 1) {
                    DrawRectangleRec(burst_time_box, GRAY);
                } else {
                    DrawRectangleRec(burst_time_box, LIGHTGRAY);
                }
            }

            DrawText(TextFormat("Process :%d", processes[i].id, processes[i].arrival_time, processes[i].burst_time), 300, 80 + i * 25, 20, BLACK);
            DrawText(TextFormat("Arrival Time: %d", processes[i].arrival_time), 440, 80 + i * 25, 20, BLACK);
            DrawText(TextFormat("Burst Time: %d", processes[i].burst_time), 620, 80 + i * 25, 20, BLACK);

            if (!is_clicked) {
                if (CheckCollisionPointRec(GetMousePosition(), arrival_time_box)) {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        std::cout << "Arrival Time" << std::endl;
                        std::cout << "Process ID: " << processes[i].id << std::endl;
                        get_input_process = i + 1;
                        get_input_arrival_time = true;
                        get_input_burst_time = false;
                        input_number = processes[i].arrival_time != 0 ? std::to_string(processes[i].arrival_time) : "";
                        is_clicked = true;
                    }
                } else if (CheckCollisionPointRec(GetMousePosition(), burst_time_box)) {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        std::cout << "Burst Time" << std::endl;
                        std::cout << "Process ID: " << processes[i].id << std::endl;
                        get_input_process = i + 1;
                        get_input_burst_time = true;
                        get_input_arrival_time = false;
                        input_number = processes[i].burst_time != 0 ? std::to_string(processes[i].burst_time) : "";
                        is_clicked = true;
                    }
                } else {
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        get_input_process = 0;
                        get_input_arrival_time = false;
                        get_input_burst_time = false;
                        input_number = "";
                        is_clicked = false;
                    }
                }
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            is_clicked = false;
        }
        if (get_input_process) {
            get_input_keyboard(input_number);
            if (get_input_arrival_time) {
                processes[get_input_process - 1].arrival_time = !input_number.empty() ? stoi(input_number) : 0;
            } else if (get_input_burst_time) {
                processes[get_input_process - 1].burst_time = !input_number.empty() ? stoi(input_number) : 0;
                processes[get_input_process - 1].remaining_time = processes[get_input_process - 1].burst_time;
            }
        }

        Rectangle start_button = {10.0f, 400.0f, 100.0f, 25.0f};
        DrawRectangleRec(start_button, LIGHTGRAY);
        DrawText("Start", start_button.x + 5, start_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), start_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            start_simulation = time_frame;
            display_menu = algorithm_type;
            sort_processes(algorithm_type);
        }

    } else if (menu == menu_t::FCFS) {
        DrawText("FCFS", 10, 10, 20, BLACK);

        DrawText("Simulation", 10, 40, 20, BLACK);
        float box_width = 0;

        if (queue_iterator < processes.size() && processes[queue_iterator].arrival_time <= (time_frame - start_simulation) / 12) {
            queued_processes.push_back(processes[queue_iterator]);
            queue_iterator++;
        }

        if (ongoing_process_iterator < processes.size() && (processes[ongoing_process_iterator].burst_time + ongoing_process_runtime) <= (time_frame - start_simulation) / 12) {
            std::reverse(ongoing_processes.begin(), ongoing_processes.end());
            ongoing_processes.push_back(processes[ongoing_process_iterator]);
            std::reverse(ongoing_processes.begin(), ongoing_processes.end());
            ongoing_process_runtime += processes[ongoing_process_iterator].burst_time;
            ongoing_process_iterator++;
        }

        DrawText("    Queue :", 40, 130, 20, BLACK);
        for (int i = 0; i < queued_processes.size(); ++i) {
            Rectangle process_box = {160.0f + i * 35.0f - (ongoing_process_iterator + 1) * 35.0f, 130.0f, 30.0f, 20.0f};
            if (ongoing_process_iterator < i) {
                DrawRectangleRec(process_box, queued_processes[i].color);
                DrawText(TextFormat("P%d", queued_processes[i].id), 160 + i * 35 - (ongoing_process_iterator + 1) * 35, 110, 20, queued_processes[i].color);
            }
        }

        float total_width = 0;
        for (const auto& p : processes) {
            total_width += p.burst_time * 6;
        }
        for (int i = 0; i <= total_width + 30; i += 30) {
            DrawLine(160 + i, 180, 160 + i, 180 + processes.size() * 25, BLACK);
            DrawText(TextFormat("%d", (i) / 6), 160 + i, 160, 10, BLACK);
        }

        // Draw process boxes and labels
        box_width = 0;
        for (int i = 0; i < processes.size(); ++i) {
            Rectangle process_box = {160.0f + box_width, 180.0f + i * 25.0f, processes[i].burst_time * 6.0f, 20.0f};

            if (i <= ongoing_process_iterator) {
                DrawText(TextFormat("Process %d", processes[i].id), 10, 180 + i * 25, 20, processes[i].color);
                DrawRectangleRec(process_box, processes[i].color);
                box_width += processes[i].burst_time * 6;
            }
        }
        DrawRectangle(160 + (time_frame - start_simulation) / 2, 180, 800, processes.size() * 25, RAYWHITE);

        // draw replay and back button
        Rectangle replay_button = {10.0f, 400.0f, 100.0f, 25.0f};
        DrawRectangleRec(replay_button, LIGHTGRAY);
        DrawText("Replay", replay_button.x + 5, replay_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), replay_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            start_simulation = time_frame;
            queue_iterator = 0;
            ongoing_process_iterator = 0;
            ongoing_process_runtime = 0;
            queued_processes.clear();
            ongoing_processes.clear();
        }
        Rectangle back_button = {10.0f, 440.0f, 100.0f, 25.0f};
        DrawRectangleRec(back_button, LIGHTGRAY);
        DrawText("Back", back_button.x + 5, back_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), back_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            display_menu = menu_t::HOME;
            start_simulation = 0;
            queue_iterator = 0;
            ongoing_process_iterator = 0;
            ongoing_process_runtime = 0;
            queued_processes.clear();
            ongoing_processes.clear();

            // SORT PROCESSES as id
            std::sort(processes.begin(), processes.end(), [](const process_t& a, const process_t& b) {
                return a.id < b.id;
            });
        }
    } else if (menu == menu_t::SJF) {
        DrawText("SJF", 10, 10, 20, BLACK);
        DrawText("Simulation", 10, 40, 20, BLACK);

        // Waktu saat ini dalam satuan waktu proses
        float current_time = (time_frame - start_simulation) / 12.0f;

        // Buat queue (antrian proses) berdasarkan waktu kedatangan dan burst time
        std::vector<process_t> ready_queue;

        for (const auto& p : processes) {
            bool is_finished = std::find_if(finished_processes.begin(), finished_processes.end(),
                                            [&](const process_t& f) { return f.id == p.id; }) != finished_processes.end();

            bool is_running = (!ongoing_processes.empty() && ongoing_processes[0].id == p.id);

            if (p.arrival_time <= current_time && !is_finished && !is_running) {
                ready_queue.push_back(p);
            }
        }

        // Urutkan ready queue berdasarkan burst time (SJF)
        std::sort(ready_queue.begin(), ready_queue.end(), [](const process_t& a, const process_t& b) {
            return a.burst_time < b.burst_time;
        });

        // Gambar ready queue
        DrawText("    Queue :", 40, 130, 20, BLACK);

        for (int i = 0; i < ready_queue.size(); ++i) {
            Rectangle box = {160.0f + i * 35.0f, 130.0f, 30.0f, 20.0f};
            DrawRectangleRec(box, ready_queue[i].color);
            DrawText(TextFormat("P%d", ready_queue[i].id), 160 + i * 35, 110, 20, ready_queue[i].color);
        }

        // Jika tidak ada proses yang sedang berjalan, pilih proses burst time terkecil yang sudah datang
        if (ongoing_processes.empty()) {
            int selected_index = -1;
            int min_burst_time = INT_MAX;

            for (int i = 0; i < processes.size(); ++i) {
                bool is_finished = std::find_if(finished_processes.begin(), finished_processes.end(),
                                                [&](const process_t& p) { return p.id == processes[i].id; }) != finished_processes.end();

                if (processes[i].arrival_time <= current_time &&
                    !is_finished &&
                    processes[i].burst_time < min_burst_time) {
                    selected_index = i;
                    min_burst_time = processes[i].burst_time;
                }
            }

            if (selected_index != -1) {
                current_process = processes[selected_index];
                ongoing_processes.push_back(current_process);
                process_start_time = current_time;
                process_end_time = current_time + current_process.burst_time;
            }
        }

        // Jika proses sedang berjalan dan sudah selesai, pindahkan ke finished
        if (!ongoing_processes.empty() && current_time >= process_end_time) {
            finished_processes.push_back(ongoing_processes.back());
            ongoing_processes.clear();
        }

        // Gambar proses yang sudah selesai (Gantt chart)
        float box_width = 0;
        for (int i = 0; i < finished_processes.size(); ++i) {
            Rectangle process_box = {160.0f + box_width, 180.0f + i * 25.0f, finished_processes[i].burst_time * 6.0f, 20.0f};
            DrawText(TextFormat("Process %d", finished_processes[i].id), 10, 180 + i * 25, 20, finished_processes[i].color);
            DrawRectangleRec(process_box, finished_processes[i].color);
            box_width += finished_processes[i].burst_time * 6;
        }

        // Gambar proses yang sedang berjalan
        if (!ongoing_processes.empty()) {
            Rectangle process_box = {160.0f + box_width, 180.0f + finished_processes.size() * 25.0f, ongoing_processes[0].burst_time * 6.0f, 20.0f};
            DrawText(TextFormat("Process %d", ongoing_processes[0].id), 10, 180 + finished_processes.size() * 25, 20, ongoing_processes[0].color);
            DrawRectangleRec(process_box, ongoing_processes[0].color);
        }

        // Gambar garis vertikal dan label waktu
        float total_width = 0;
        for (const auto& p : processes) {
            total_width += p.burst_time * 6;
        }
        for (int i = 0; i <= total_width + 30; i += 30) {
            DrawLine(160 + i, 180, 160 + i, 180 + processes.size() * 25, BLACK);
            DrawText(TextFormat("%d", (i) / 6), 160 + i, 160, 10, BLACK);
        }

        // Highlight waktu sekarang
        DrawRectangle(160 + (time_frame - start_simulation) / 2, 180, 800, processes.size() * 25, RAYWHITE);

        // Tombol Replay
        Rectangle replay_button = {10, 400, 100, 25};
        DrawRectangleRec(replay_button, LIGHTGRAY);
        DrawText("Replay", replay_button.x + 5, replay_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), replay_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            start_simulation = time_frame;
            queued_processes.clear();
            ongoing_processes.clear();
            finished_processes.clear();
            process_start_time = 0;
            process_end_time = 0;
        }

        // Tombol Back
        Rectangle back_button = {10, 440, 100, 25};
        DrawRectangleRec(back_button, LIGHTGRAY);
        DrawText("Back", back_button.x + 5, back_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), back_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            display_menu = menu_t::HOME;
            start_simulation = 0;
            queued_processes.clear();
            ongoing_processes.clear();
            finished_processes.clear();
            process_start_time = 0;
            process_end_time = 0;

            std::sort(processes.begin(), processes.end(), [](const process_t& a, const process_t& b) {
                return a.id < b.id;
            });
        }

    } else if (menu == menu_t::RR) {
        DrawText("RR", 10, 10, 20, BLACK);

        float total_width = 0;
        for (const auto& p : processes) {
            total_width += p.burst_time * 6;
        }
        for (int i = 0; i <= total_width + 30; i += 30) {
            DrawLine(160 + i, 180, 160 + i, 180 + processes.size() * 25, BLACK);
            DrawText(TextFormat("%d", (i) / 6), 160 + i, 160, 10, BLACK);
        }

        float box_width = 0;

        static std::vector<process_t> process_in;
        static std::vector<RR_process_t> rr_process_img;
        static std::vector<process_t> rr_queue;
        process_in.clear();
        for (int i = 0; i < processes.size(); ++i) {
            if (processes[i].arrival_time <= (time_frame - start_simulation) / 12) {
                process_in.push_back(processes[i]);
            }
        }

        rr_queue.clear();
        rr_queue = std::vector<process_t>(processes.begin(), processes.end());
        rr_process_img.clear();
        bool simulation_rendered = false;
        while (!simulation_rendered) {
            simulation_rendered = true;
            for (int i = 0; i < rr_queue.size(); ++i) {
                if (rr_queue[i].remaining_time > 0) {
                    if (rr_queue[i].remaining_time > 5) {
                        rr_process_img.push_back({rr_queue[i].id, 5});
                        rr_queue[i].remaining_time -= 5;
                        simulation_rendered = false;  // Continue simulating until all processes are finished
                    } else {
                        rr_process_img.push_back({rr_queue[i].id, rr_queue[i].remaining_time});
                        rr_queue[i].remaining_time = 0;
                    }
                }
            }
        }

        int colide_id = -1;
        std::set<int> processed_ids;
        for (int i = 0; i < rr_process_img.size(); ++i) {
            for (int j = 0; j < processes.size(); ++j) {
                Rectangle process_box = {160.0f + box_width, 180.0f + j * 25.0f, rr_process_img[i].burst_time * 6.0f, 20.0f};
                if (CheckCollisionRecs(process_box, {(float)160 + (time_frame - start_simulation) / 2, 180, 1, 800})) {
                    colide_id = rr_process_img[i].id;
                }
                if (CheckCollisionRecs(process_box, {(float)160 + (time_frame - start_simulation) / 2, 180, 800, (float)processes.size() * 25})) {
                    processed_ids.insert(rr_process_img[i].id);
                }
                if (processes[j].id == rr_process_img[i].id) {
                    DrawText(TextFormat("Process %d", rr_process_img[i].id), 10, 180 + j * 25, 20, processes[j].color);
                    DrawRectangleRec(process_box, processes[j].color);
                    break;
                }
            }

            box_width += rr_process_img[i].burst_time * 6;
        }

        std::vector<process_t> robin_queue;
        for (int i = 0; i < processes.size(); ++i) {
            if (processed_ids.find(processes[i].id) != processed_ids.end() && processes[i].id != colide_id) {
                for (auto& p : process_in) {
                    if (p.id == processes[i].id) {
                        robin_queue.push_back(p);
                    }
                }
                // robin_queue.push_back(processes[i]);
            }
        }

        DrawText("    Queue :", 40, 130, 20, BLACK);

        for (int k = 0; k < robin_queue.size(); ++k) {
            // if (colide_id != robin_queue[k].id) {
            Rectangle queue_box = {160.0f + k * 35.0f, 130.0f, 30.0f, 20.0f};
            DrawRectangleRec(queue_box, robin_queue[k].color);
            DrawText(TextFormat("P%d", robin_queue[k].id), queue_box.x, queue_box.y - 20, 20, robin_queue[k].color);
        }

        DrawRectangle(160 + (time_frame - start_simulation) / 2, 180, 800, processes.size() * 25, RAYWHITE);
        // draw queue

        // Add replay and back button logic if needed (similar to other algorithms)
        Rectangle replay_button = {10, 400, 100, 25};
        DrawRectangleRec(replay_button, LIGHTGRAY);
        DrawText("Replay", replay_button.x + 5, replay_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), replay_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            start_simulation = time_frame;
            rr_process_img.clear();
            rr_queue.clear();
            for (auto& p : processes) {
                p.remaining_time = p.burst_time;  // Reset remaining time
            }
        }
        Rectangle back_button = {10, 440, 100, 25};
        DrawRectangleRec(back_button, LIGHTGRAY);
        DrawText("Back", back_button.x + 5, back_button.y + 5, 20, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), back_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            display_menu = menu_t::HOME;
            start_simulation = 0;
            rr_process_img.clear();
            rr_queue.clear();
            for (auto& p : processes) {
                p.remaining_time = p.burst_time;  // Reset remaining time
            }

            // SORT PROCESSES as id
            std::sort(processes.begin(), processes.end(), [](const process_t& a, const process_t& b) {
                return a.id < b.id;
            });
        }
    }

    //     DrawText("RR", 10, 10, 20, BLACK);
    //     // Implement RR logic here
    //     // Draw vertical lines and time labels first (behind boxes)
    //     float total_width = 0;
    //     for (const auto& p : processes) {
    //         total_width += p.burst_time * 6;
    //     }
    //     for (int i = 0; i <= total_width + 30; i += 30) {
    //         DrawLine(160 + i, 180, 160 + i, 180 + processes.size() * 25, BLACK);
    //         DrawText(TextFormat("%d", (i) / 6), 160 + i, 160, 10, BLACK);
    //     }

    //     // Round Robin simulation
    //     float box_width = 0;
    //     const int quantum = 5;  // time quantum
    //     static std::vector<RR_process_t> rr_process_img;
    //     static bool rr_simulated = false;
    //     if (!rr_simulated) {
    //         rr_process_img.clear();
    //         std::deque<process_t> rr_queue(processes.begin(), processes.end());
    //         std::vector<int> remaining_time;
    //         for (const auto& p : processes) remaining_time.push_back(p.burst_time);
    //         int time = 0;
    //         while (!rr_queue.empty()) {
    //             process_t proc = rr_queue.front();
    //             rr_queue.pop_front();
    //             int idx = proc.id;
    //             int exec_time = std::min(quantum, remaining_time[idx]);
    //             if (exec_time > 0) {
    //                 rr_process_img.push_back({proc.id, exec_time});
    //                 remaining_time[idx] -= exec_time;
    //                 time += exec_time;
    //                 if (remaining_time[idx] > 0) {
    //                     rr_queue.push_back(proc);
    //                 }
    //             }
    //         }
    //         rr_simulated = true;
    //     }

    //     for (int i = 0; i < rr_process_img.size(); ++i) {
    //         Rectangle process_box = {160 + box_width, 180 + rr_process_img[i].id * 25, rr_process_img[i].burst_time * 6, 20};
    //         DrawText(TextFormat("Process %d", rr_process_img[i].id), 10, 180 + rr_process_img[i].id * 25, 20, processes[rr_process_img[i].id].color);
    //         DrawRectangleRec(process_box, processes[rr_process_img[i].id].color);
    //         box_width += rr_process_img[i].burst_time * 6;
    //     }
    //     // Add replay and back button logic if needed (similar to other algorithms)
    // }
}

int main() {
    // Initialize the window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Process Scheduling Simulator");

    // Set the target FPS
    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        display(display_menu);
        EndDrawing();
        time_frame++;
    }

    // Close the window
    CloseWindow();

    return 0;
}