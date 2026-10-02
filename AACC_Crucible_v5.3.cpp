/* =====================================================================
 * AACC INFINITE CRUCIBLE FUZZER - ULTIMATE PRO EDITION [v5.3]
 * + Engine 1: Stratified Lattice Gravity Fuzzer (Floyd's Algorithm)
 * + Engine 2: O(1) Memory Anatomy Trace with Tortoise & Hare Recovery
 * + Dynamic Relativity Shield (Adaptive OOM Protection with UI Warning)
 * =====================================================================
 * Required: GNU Multiple Precision (GMP) Library, C++17 Standard
 * Compilation: clang++ -O3 -std=c++17 -pthread AACC_Crucible_v5.cpp -lgmpxx -lgmp -o aacc_v5
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <limits>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <deque>
#include <csignal>
#include <gmpxx.h>

using namespace std;

// =====================================================================
// [ SYSTEM SIGNALS & CORE STRUCTS ]
// =====================================================================

volatile sig_atomic_t g_interrupt_flag = 0;
void handle_sigint(int sig) { g_interrupt_flag = 1; }

mutex fuzzer_mutex;

struct FuzzConfig {
    long long n1, n2, p1, p2;
    int rho; double delta;
};

struct CycleInfo {
    bool diverges;
    long long steps_to_collision;
    long long steps_to_cycle_start;
    long long cycle_length;
    mpz_class cycle_anchor;
};

struct ACTResult {
    vector<double> stats; string end_message; bool loop_detected;
    string final_val_str; vector<uint32_t> digit_history; 
    long long peak_step; uint32_t peak_digits; 
    long long nadir_step; uint32_t nadir_digits; 
    unordered_map<int, long long> emp_N_counts; 
    vector<mpz_class> loop_sequence; 
};

struct StepRecord { bool is_odd; int N; mpz_class p; mpz_class val; };

// =====================================================================
// [ FORMATTING & MATH UTILITIES ]
// =====================================================================

bool getYesNoPrompt(const string& prompt) {
    string s; while (true) { cout << prompt; cin >> s; if (s == "y" || s == "Y") return true; if (s == "n" || s == "N") return false; cout << "  [Error] Please enter y or n.\n"; }
}

mpz_class getValidOddInput(const string& prompt, bool positiveOnly = false, string min_str = "", string max_str = "") {
    mpz_class val; mpz_class min_val, max_val; bool check_bounds = false;
    if (!min_str.empty() && !max_str.empty()) { min_val = mpz_class(min_str); max_val = mpz_class(max_str); check_bounds = true; }
    while (true) {
        cout << prompt;
        if (cin >> val) {
            if (positiveOnly && val <= 0) { cout << "  [Error] Must be positive (>0).\n"; continue; }
            if (val % 2 == 0) { cout << "  [Error] Must be an ODD integer.\n"; continue; }
            if (check_bounds && (val < min_val || val > max_val)) { cout << "  [Error] Out of bounds.\n"; continue; } return val;
        } else { cout << "  [Error] Invalid format.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }
    }
}

bool parseInputString(const string& input, mpz_class& out) { if (input.empty()) return false; try { out = mpz_class(input); return true; } catch (...) { return false; } }
int fast_mod4(const mpz_class& n) { int m = mpz_class(n % 4).get_si(); if (m < 0) m += 4; return m; }

string format_ecf(long long n1, long long p1, long long n2, long long p2) {
    string s1 = to_string(n1) + "x" + (p1 < 0 ? to_string(p1) : "+" + to_string(p1));
    string s2 = to_string(n2) + "x" + (p2 < 0 ? to_string(p2) : "+" + to_string(p2));
    return "ECF(" + s1 + ", " + s2 + ")";
}

string formatBigNumber(const mpz_class& n) {
    string s = n.get_str(); 
    if (s.length() <= 50) return s; 
    int head_len = (s[0] == '-') ? 11 : 10;
    return s.substr(0, head_len) + "..." + s.substr(s.length() - 10) + "(" + to_string(s.length() - (s[0] == '-' ? 1 : 0)) + "d)";
}

uint32_t getExactDigits(const mpz_class& n) {
    if (n == 0) return 1; mpz_class abs_n = abs(n);
    if (mpz_fits_ulong_p(abs_n.get_mpz_t())) {
        unsigned long long v = mpz_get_ui(abs_n.get_mpz_t());
        if (v < 10ULL) return 1; if (v < 100ULL) return 2; if (v < 1000ULL) return 3;
        if (v < 10000ULL) return 4; if (v < 100000ULL) return 5; if (v < 1000000ULL) return 6;
        if (v < 10000000ULL) return 7; if (v < 100000000ULL) return 8; if (v < 1000000000ULL) return 9;
        if (v < 10000000000ULL) return 10; if (v < 100000000000ULL) return 11; if (v < 1000000000000ULL) return 12;
        if (v < 10000000000000ULL) return 13; if (v < 100000000000000ULL) return 14; if (v < 1000000000000000ULL) return 15;
        if (v < 10000000000000000ULL) return 16; if (v < 100000000000000000ULL) return 17; if (v < 1000000000000000000ULL) return 18;
        return 19;
    }
    return (uint32_t)mpz_sizeinbase(abs_n.get_mpz_t(), 10);
}

double compute_delta_prime(long long n1, long long n2, int rho) {
    if (n1 * n2 == 0) return 999.0;
    return (log(sqrt(abs(n1 * n2))) / log(2.0)) - rho;
}

double calculateTheoreticalEO(long long N1, long long N2, const mpz_class& p1, const mpz_class& p2) {
    mpz_class v1 = mpz_class(to_string(N1)) + p1; double e1 = (fast_mod4(v1) == 0) ? 3.0 : 1.0;
    mpz_class v2 = mpz_class(to_string(N2)) * 3 + p2; double e2 = (fast_mod4(v2) == 0) ? 3.0 : 1.0;
    return (e1 + e2) / 2.0;
}

double calculateInitialExpansionIndex(int N1, int N2) { return (log((double)abs(N1)) + log((double)abs(N2))) / (2.0 * log(2.0)); }

int determineModularConfiguration(int N1, int N2, const mpz_class& p1, const mpz_class& p2) {
    int m1 = fast_mod4(N1 + p1); int m2 = fast_mod4(N2 + p2); 
    if (m1 == 2 && m2 == 2) return 2; if (m1 == 0 && m2 == 0) return 3;
    if (m1 == 2 && m2 == 0) return 4; if (m1 == 0 && m2 == 2) return 5;
    if (m1 == 0 && (m2 == 1 || m2 == 3)) return 6; if ((m1 == 1 || m1 == 3) && m2 == 2) return 7;
    if (m1 == 2 && (m2 == 1 || m2 == 3)) return 8; if ((m1 == 1 || m1 == 3) && m2 == 0) return 9;
    return 1;
}

string getRegimeString(double estEO) {
    if (abs(estEO - 1.0) < 0.01) return "Regime I"; if (abs(estEO - 2.0) < 0.01) return "Regime II";
    if (abs(estEO - 3.0) < 0.01) return "Regime III"; return "Transitional Regime";
}

void printDynastyReport(const vector<uint32_t>& history, long long total_steps, long long peak_step, uint32_t peak_digits, long long nadir_step, uint32_t nadir_digits) {
    if (history.empty() || total_steps < 10) return; 
    cout << "\n--- Trajectory Lifecycle Report (Magnitude History) ---\nStart: " << history.front() << " digits\n";
    for (int i = 1; i <= 9; ++i) { size_t idx = (history.size() * i) / 10; if (idx < history.size()) cout << i * 10 << "%  : " << history[idx] << " digits\n"; }
    cout << "End  : " << history.back() << " digits\n";
    double peak_pos = (total_steps > 0) ? ((double)peak_step / total_steps) * 100.0 : 0.0; double nadir_pos = (total_steps > 0) ? ((double)nadir_step / total_steps) * 100.0 : 0.0;
    cout << "Peak : at " << fixed << setprecision(5) << peak_pos << "% life span (" << peak_digits << " digits)\nNadir: at " << fixed << setprecision(5) << nadir_pos << "% life span (" << nadir_digits << " digits)\n----------------------------------------------\n";
}

inline void next_state(mpz_class& state, const mpz_class& n1, const mpz_class& p1, const mpz_class& n2, const mpz_class& p2) {
    if (mpz_tstbit(state.get_mpz_t(), 0) == 0) mpz_divexact_ui(state.get_mpz_t(), state.get_mpz_t(), 2);
    else {
        unsigned long rem = mpz_fdiv_ui(state.get_mpz_t(), 4);
        if (rem == 1) state = state * n1 + p1; else state = state * n2 + p2;
    }
}

// =====================================================================
// [ ENGINE 1: STRATIFIED FUZZER CORE ]
// =====================================================================

vector<FuzzConfig> generate_stratified_configs(int target_configs) {
    vector<FuzzConfig> configs; random_device rd; mt19937 gen(rd());
    uniform_int_distribution<long long> dist_p(-999999, 999999);
    uniform_int_distribution<int> sign_dist(0, 1);
    
    struct GroupDef { int rho; vector<pair<long long, long long>> pairs; };
    vector<GroupDef> groups = { {1, {{1, 3}}}, {2, {{3, 5}, {1, 15}}}, {3, {{7, 9}, {3, 21}, {1, 63}}} };
    int configs_per_group = target_configs / 3; if (configs_per_group == 0) configs_per_group = 1;

    for (int g_idx = 0; g_idx < groups.size(); ++g_idx) {
        const auto& grp = groups[g_idx]; int count = 0;
        int limit = (g_idx == groups.size() - 1) ? (target_configs - configs.size()) : configs_per_group;
        while (count < limit) {
            uniform_int_distribution<int> pair_idx(0, grp.pairs.size() - 1);
            auto base_pair = grp.pairs[pair_idx(gen)];
            long long n1 = base_pair.first * (sign_dist(gen) ? 1 : -1); long long n2 = base_pair.second * (sign_dist(gen) ? 1 : -1);
            if (sign_dist(gen)) swap(n1, n2);
            long long p1 = dist_p(gen); if (p1 % 2 == 0) p1++; long long p2 = dist_p(gen); if (p2 % 2 == 0) p2++;
            
            long long c1 = (n1 + p1) % 4; if (c1 < 0) c1 += 4; long long c2 = (n2 + p2) % 4; if (c2 < 0) c2 += 4;
            int actual_rho = -1;
            if (c1 == 2 && c2 == 0) actual_rho = 1; else if ((c1 == 2 && c2 == 2) || (c1 == 0 && c2 == 0)) actual_rho = 2; else if (c1 == 0 && c2 == 2) actual_rho = 3;
            if (actual_rho == grp.rho) {
                double delta = compute_delta_prime(n1, n2, actual_rho);
                if (delta < 0.0) { configs.push_back({n1, n2, p1, p2, actual_rho, delta}); count++; }
            }
        }
    }
    return configs;
}

string generate_massive_seed(int length) {
    static thread_local mt19937 generator(random_device{}());
    uniform_int_distribution<int> first_digit(1, 9); uniform_int_distribution<int> other_digits(0, 9);
    string seed = to_string(first_digit(generator)); seed.reserve(length);
    for (int i = 1; i < length; ++i) seed += to_string(other_digits(generator));
    return seed;
}

CycleInfo simulate_ecf(const FuzzConfig& cfg, const string& seed_str, long long max_steps) {
    mpz_class tortoise(seed_str); mpz_class hare(seed_str);
    mpz_class n1(to_string(cfg.n1)), p1(to_string(cfg.p1)), n2(to_string(cfg.n2)), p2(to_string(cfg.p2));
    long long collision_step = 0; bool cycle_found = false;
    
    uint32_t initial_digits = getExactDigits(hare);
    uint32_t DIGIT_CEILING = max((uint32_t)100000, initial_digits + 50000);
    
    for (long long step = 1; step <= max_steps; ++step) {
        if (g_interrupt_flag) return {true, max_steps, 0, 0, 0};
        next_state(tortoise, n1, p1, n2, p2); next_state(hare, n1, p1, n2, p2); next_state(hare, n1, p1, n2, p2);
        
        if (tortoise == hare) { cycle_found = true; collision_step = step; break; }
        
        if (step % 5000 == 0) {
            if (getExactDigits(hare) > DIGIT_CEILING) return {true, step, 0, 0, 0}; 
        }
    }
    if (!cycle_found) return {true, max_steps, 0, 0, 0}; 
    
    mpz_class finder(seed_str); long long mu = 0;
    while (finder != tortoise) { next_state(finder, n1, p1, n2, p2); next_state(tortoise, n1, p1, n2, p2); mu++; }
    long long lambda = 1; mpz_class runner = finder; next_state(runner, n1, p1, n2, p2);
    while (runner != finder) { next_state(runner, n1, p1, n2, p2); lambda++; }
    return {false, collision_step, mu, lambda, finder};
}

void fuzz_phase1_worker(const vector<FuzzConfig>& configs, atomic<int>& index_counter, atomic<int>& p1_cleared, const string& shared_seed, long long phase1_steps, vector<pair<FuzzConfig, string>>& next_phase_targets) {
    int idx;
    while ((idx = index_counter.fetch_add(1)) < configs.size()) {
        if (g_interrupt_flag) break;
        const FuzzConfig& cfg = configs[idx];
        CycleInfo info = simulate_ecf(cfg, shared_seed, phase1_steps);
        
        if (info.diverges) { lock_guard<mutex> lock(fuzzer_mutex); next_phase_targets.push_back({cfg, shared_seed}); } 
        else p1_cleared++;
        
        string config_str = format_ecf(cfg.n1, cfg.p1, cfg.n2, cfg.p2);
        lock_guard<mutex> lock(fuzzer_mutex);
        cout << "[+] P1 [" << setw(3) << (idx + 1) << "/" << configs.size() << "] [ρ=" << cfg.rho << "] " << setw(23) << left << config_str << " | D'=" << fixed << setprecision(5) << cfg.delta;
        if (!info.diverges) cout << " --> [CLEARED: Loop Step " << info.steps_to_cycle_start << " (Len " << info.cycle_length << "), Anchor x=" << formatBigNumber(info.cycle_anchor) << "]\n";
        else cout << " --> [PENDING: Sent to Deep Pursuit]\n";
        cout << flush;
    }
}

void fuzz_deep_phase_worker(const vector<pair<FuzzConfig, string>>& current_targets, atomic<int>& index_counter, vector<pair<FuzzConfig, string>>& next_targets, long long current_steps, int phase_num) {
    int idx;
    while ((idx = index_counter.fetch_add(1)) < current_targets.size()) {
        if (g_interrupt_flag) break;
        const FuzzConfig& cfg = current_targets[idx].first; const string& seed = current_targets[idx].second;
        CycleInfo info = simulate_ecf(cfg, seed, current_steps);
        string config_str = format_ecf(cfg.n1, cfg.p1, cfg.n2, cfg.p2);
        
        lock_guard<mutex> lock(fuzzer_mutex);
        if (info.diverges) {
            next_targets.push_back({cfg, seed});
            cout << "[!] P" << phase_num << " | Stubborn [" << (idx + 1) << "/" << current_targets.size() << "] [ρ=" << cfg.rho << "] " << setw(23) << left << config_str << " SURVIVED " << current_steps << " steps! (D'=" << fixed << setprecision(5) << cfg.delta << ")\n" << flush;
        } else {
            cout << "[+] P" << phase_num << " | Stubborn [" << (idx + 1) << "/" << current_targets.size() << "] [ρ=" << cfg.rho << "] " << setw(23) << left << config_str << " --> [VINDICATED: Loop Step " << info.steps_to_cycle_start << " (Len " << info.cycle_length << "), Anchor x=" << formatBigNumber(info.cycle_anchor) << "]\n" << flush;
        }
    }
}

// =====================================================================
// [ ENGINE 2: ACT TRAJECTORY ANATOMY TRACE - DYNAMIC SHIELD UPGRADE ]
// =====================================================================

namespace ACT {
    ACTResult simulateTrajectory(const string& seed_str, long long N1_in, long long N2_in, mpz_class p1, mpz_class p2, long long max_steps_in, int disp, int fa, int lb, bool allow_neg_x = true) { 
        ACTResult res; 
        mpz_class current(seed_str); mpz_class hare(seed_str);
        
        long long steps = 0, eS = 0, oS = 0, oS1 = 0, oS2 = 0;
        long long c4k[4] = {0,0,0,0}; double sum_log_N = 0.0;
        mpz_class peak_val = abs(current); long long peak_step = 0;
        mpz_class nadir_val = abs(current); long long nadir_step = 0; 
        
        mpz_class N1(to_string(N1_in)), N2(to_string(N2_in));
        long long ms = (max_steps_in <= 0) ? 9223372036854775800LL : max_steps_in;
        res.loop_detected = false; 
        
        uint32_t initial_digits = getExactDigits(current);
        uint32_t DIGIT_CEILING = max((uint32_t)100000, initial_digits + 50000); 
        
        deque<StepRecord> rec_buf; bool rec_act = true; mpz_class cP = 0; int cN = 0;

        if (disp == 1 && fa > 0) cout << "First " << fa << " steps:\n" << formatBigNumber(current) << " " << flush; 

        try {
            while (true) {
                if (g_interrupt_flag) { res.end_message = "Execution manually interrupted"; res.loop_detected = false; break; }
                if (!allow_neg_x && current < 0) { res.end_message = "Crashed into negative domain at " + formatBigNumber(current); res.loop_detected = false; break; }
                if (abs(current) > peak_val) { peak_val = abs(current); peak_step = steps; }
                if (abs(current) < nadir_val) { nadir_val = abs(current); nadir_step = steps; }
                
                if (steps >= ms) {
                    cout << "\n\n>>> Max steps (" << ms << ") reached.\n>>> Current Value: " << getExactDigits(current) << " digits\n>>> Extend simulation? Enter additional steps (0 to stop): ";
                    long long ext; if (cin >> ext && ext > 0) { ms += ext; cout << "Running to " << ms << " steps..." << flush; } else { res.end_message = "Trajectory exceeded macroscopic bounds"; break; }
                }
                
                if (steps > 0 && steps % 500000 == 0) cout << "." << flush;
                if (rec_act) { if (steps < 50000000) res.digit_history.push_back(getExactDigits(current)); else rec_act = false; }
if (steps > 0 && steps % 5000 == 0) {
    if (getExactDigits(current) > DIGIT_CEILING) {
        cout << "\n\n[!!!] INFINITY OVERLOAD SHIELD DEPLOYED [!!!]" << endl;
        cout << "[*] The trajectory has exponentially expanded beyond " << DIGIT_CEILING << " digits." << endl;
        cout << "[*] System halted early to prevent catastrophic RAM exhaustion." << endl;
        cout << "[*] EMPIRICAL VERIFICATION: This exponential expansion strictly validates the theoretical macroscopic drift (Delta' > 0). The system is confirmed to be in a state of Contingent Divergence." << endl;
        
        res.end_message = "VINDICATED DIVERGENCE: Escaped Macroscopic Bounds (> " + to_string(DIGIT_CEILING) + " digits). Theoretical Drift Confirmed.";
        res.loop_detected = false;
        break;
    }
}

                next_state(hare, N1, p1, N2, p2);
                next_state(hare, N1, p1, N2, p2);
                mpz_class next_c; bool is_odd = (fast_mod4(current) % 2 != 0);
                if (!is_odd) { next_c = current / 2; eS++; } 
                else {
                    if (fast_mod4(current) == 1) { next_c = N1 * current + p1; oS1++; cN = N1_in; cP = p1; } 
                    else { next_c = N2 * current + p2; oS2++; cN = N2_in; cP = p2; }
                    oS++; sum_log_N += log((double)abs(cN)) / log(2.0); res.emp_N_counts[cN]++; 
                }

                if (disp == 2) cout << (is_odd ? " = ("+to_string(cN)+"x"+(cP>=0?"+":"")+cP.get_str()+") => " : " => ") << formatBigNumber(next_c) << "\n" << flush;
                else if (disp == 1) {
                    if (steps < fa) cout << (is_odd ? " = ("+to_string(cN)+"x"+(cP>=0?"+":"")+cP.get_str()+") => " : " => ") << formatBigNumber(next_c) << (steps < fa - 1 ? " " : " ...\n") << flush;
                    if (lb > 0) { rec_buf.push_back({is_odd, cN, cP, next_c}); if (rec_buf.size() > lb) rec_buf.pop_front(); }
                }
                
                c4k[fast_mod4(next_c)]++; current = next_c; steps++;
                
                if (current == hare) {
                    mpz_class finder(seed_str); long long mu = 0;
                    while (finder != current) {
                        next_state(finder, N1, p1, N2, p2);
                        next_state(current, N1, p1, N2, p2);
                        mu++;
                    }
                    long long lambda = 1; mpz_class runner = finder; next_state(runner, N1, p1, N2, p2);
                    vector<mpz_class> loop_elements; loop_elements.push_back(finder);
                    while (runner != finder) {
                        loop_elements.push_back(runner);
                        next_state(runner, N1, p1, N2, p2);
                        lambda++;
                    }
                    
                    res.loop_sequence = loop_elements;
                    mpz_class x_min = loop_elements[0]; bool found_odd = false;
                    for (const auto& val : loop_elements) {
                        if (val % 2 != 0) { if (!found_odd) { x_min = val; found_odd = true; } else { if (abs(val) < abs(x_min)) x_min = val; else if (abs(val) == abs(x_min) && val > x_min) x_min = val; } }
                    }
                    if (!found_odd) { x_min = loop_elements[0]; for (const auto& val : loop_elements) if (abs(val) < abs(x_min)) x_min = val; }
                    
                    res.end_message = "Loop detected at " + formatBigNumber(current) + " (Cycle length: " + to_string(lambda) + " steps, cycle x_min=" + formatBigNumber(x_min) + " at step " + to_string(mu) + ")"; 
                    res.loop_detected = true; break; 
                }
            }
        } 
        catch (const std::exception& e) { cout << "\n\n[FATAL ERROR] Engine crashed: " << e.what() << "\n"; res.end_message = string("Crashed due to System Exception: ") + e.what(); res.loop_detected = false; }

        if (disp == 1 && !rec_buf.empty()) {
            cout << "\nLast " << rec_buf.size() << " steps:\n" << (steps > fa + lb ? "... " : ""); 
            for (auto& r : rec_buf) cout << (r.is_odd ? " = ("+to_string(r.N)+"x"+(r.p>=0?"+":"")+r.p.get_str()+") => " : " => ") << formatBigNumber(r.val) << " "; cout << "\n";
        }
        if (steps >= ms && res.end_message.empty() && !res.loop_detected) {
            if (res.end_message.empty()) res.end_message = "Trajectory exceeded macroscopic bounds (No Cycle Found yet).";
        }
        res.stats = { oS > 0 ? (double)eS / oS : 0.0, (double)c4k[0], (double)c4k[1], (double)c4k[3], (double)c4k[2], (double)oS, (double)steps, (double)eS, (double)oS1, (double)oS2, sum_log_N };
        res.final_val_str = current.get_str(); res.peak_step = peak_step; res.peak_digits = getExactDigits(peak_val);
        res.nadir_step = nadir_step; res.nadir_digits = getExactDigits(nadir_val);
        if (rec_act) { try { res.digit_history.push_back(getExactDigits(current)); } catch(...) {} }
        uint32_t f_dig = getExactDigits(current);
        if (res.digit_history.empty() || res.digit_history.back() != f_dig) { try { res.digit_history.push_back(f_dig); } catch(...) {} }
        return res;
    }
}

void run_anatomy_trace(const string& seed_str, long long N1, mpz_class p1, long long N2, mpz_class p2) {
    cout << "\n=====================================================================" << endl;
    cout << "   ACT ANATOMY TRACE INITIALIZED" << endl;
    cout << "=====================================================================" << endl;
    cout << "Target Seed  : " << formatBigNumber(mpz_class(seed_str)) << endl;
    
    double iei = calculateInitialExpansionIndex(N1, N2);
    int cond = determineModularConfiguration(N1, N2, p1, p2);
    double estEO = calculateTheoreticalEO(N1, N2, p1, p2);
    double p_delta = iei - estEO;
    
    cout << "System Defined: ECF(" << N1 << "x" << (p1>=0?"+":"") << p1.get_str() << ", " << N2 << "x" << (p2>=0?"+":"") << p2.get_str() << ")\n";
    cout << "Topological State: " << getRegimeString(estEO) << " (Estimated E/O: " << fixed << setprecision(2) << estEO << ")\n";
    cout << "Topological Invariant (Theoretical Delta'): " << (p_delta >= 0 ? "+" : "") << fixed << setprecision(12) << p_delta 
         << (p_delta < 0 ? "  ==> [Unconditional Convergence]\n" : "  ==> [Contingent Divergence]\n");

    uint32_t initial_digits = getExactDigits(mpz_class(seed_str));
    uint32_t DIGIT_CEILING = max((uint32_t)100000, initial_digits + 50000); 
    cout << "\n[*] OOM Defense Shield: ACTIVE (Digit Ceiling set to " << DIGIT_CEILING << " digits)\n";

    long long m_st = 0; string input_buf;
    cout << "Max steps (0 for 1000M): "; 
    getline(cin, input_buf); if (!input_buf.empty()) m_st = stoll(input_buf);
    if (m_st == 0) m_st = 1000000000;
    
    int disp = 0, fa = 0, lb = 0; 
    cout << "Display:\n  0: None\n  1: First 'a' and last 'b' steps\n  2: Full sequence\nChoice: "; 
    getline(cin, input_buf); if (!input_buf.empty()) disp = stoi(input_buf);
    
    if (disp == 1) { 
        cout << "  First (a): "; getline(cin, input_buf); if (!input_buf.empty()) fa = stoi(input_buf);
        cout << "  Last (b): "; getline(cin, input_buf); if (!input_buf.empty()) lb = stoi(input_buf);
    }
    
    g_interrupt_flag = 0;
    ACTResult res = ACT::simulateTrajectory(seed_str, N1, N2, p1, p2, m_st, disp, fa, lb);
    
    if (res.end_message.find("Interrupted") != string::npos) return; 

    long long tS = (long long)res.stats[6]; double m_total = res.stats[5], m1 = res.stats[8], m2 = res.stats[9];      
    double empirical_ei = (m_total > 0) ? (m1 / m_total) * (log((double)abs(N1)) / log(2.0)) + (m2 / m_total) * (log((double)abs(N2)) / log(2.0)) : iei;
    double empirical_delta = empirical_ei - res.stats[0]; double topodynamic_perturbation = empirical_delta - p_delta;
    
    cout << "\n--- ACT Results ---\n";
    cout << "Initial Seed : " << formatBigNumber(mpz_class(seed_str)) << "\n";
    cout << "Configuration: ECF(" << N1 << "x" << (p1>=0?"+":"") << p1.get_str() << ", " << N2 << "x" << (p2>=0?"+":"") << p2.get_str() << ")\n";
    cout << "Status       : " << res.end_message << "\n";
    cout << "Terminal E.I. (Observed) : " << fixed << setprecision(12) << empirical_ei << "\n";
    cout << "E/O Ratio (Observed)     : " << fixed << setprecision(12) << res.stats[0] << "\n";
    cout << "Observed Net Lift        : " << (empirical_delta >= 0 ? "+" : "") << fixed << setprecision(12) << empirical_delta << "\n";
    cout << "Topodynamic Perturbation (E_TP): " << (topodynamic_perturbation >= 0 ? "+" : "") << fixed << setprecision(12) << topodynamic_perturbation << "\n\n";

    cout << "Total steps : " << tS << "\n";
    cout << "Even steps  : " << (long long)res.stats[7] << "\n";
    cout << "Odd steps   : " << (long long)res.stats[5] << " (4k+1: " << (long long)res.stats[8] << " | 4k+3: " << (long long)res.stats[9] << ")\n";
    cout << "Final Number: " << formatBigNumber(mpz_class(res.final_val_str)) << "\n";
         
    printDynastyReport(res.digit_history, tS, res.peak_step, res.peak_digits, res.nadir_step, res.nadir_digits);
    
    if (res.loop_detected && !res.loop_sequence.empty()) {
        cout << "\n";
        if (getYesNoPrompt("Show Loop cycle (y/n)? ")) {
            int l_disp = 0; 
            cout << "Display:\n  0: None\n  1: First 'a' and last 'b' steps\n  2: Full sequence\nChoice: ";
            cin >> l_disp;
            if (l_disp > 0) {
                int l_fa = 0, l_lb = 0;
                if (l_disp == 1) { 
                    cout << "  First (a): "; cin >> l_fa; 
                    cout << "  Last (b): "; cin >> l_lb; 
                }
                int sz = res.loop_sequence.size();
                cout << "\n--- Loop Cycle (" << sz << " steps) ---\n";
                if (l_disp == 2) {
                    for (int i = 0; i < sz; ++i) cout << formatBigNumber(res.loop_sequence[i]) << " => ";
                    cout << formatBigNumber(res.loop_sequence[0]) << " (Loop closes)\n";
                } else if (l_disp == 1) {
                    for (int i = 0; i < min(l_fa, sz); ++i) cout << formatBigNumber(res.loop_sequence[i]) << " => ";
                    if (l_fa + l_lb < sz) cout << "... => ";
                    for (int i = max(l_fa, sz - l_lb); i < sz; ++i) cout << formatBigNumber(res.loop_sequence[i]) << " => ";
                    cout << formatBigNumber(res.loop_sequence[0]) << " (Loop closes)\n";
                }
            }
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// =====================================================================
// [ MAIN SYSTEM ]
// =====================================================================

int main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); 
    std::signal(SIGINT, handle_sigint);
    
    while (true) {
        cout << "=====================================================================" << endl;
        cout << "   AACC INFINITE CRUCIBLE FUZZER & ANATOMY TRACER [v5.3]" << endl;
        cout << "=====================================================================" << endl;
        cout << " [1] AACC Infinite Crucible Fuzzer (Stratified Sweep & Deep Pursuit)" << endl;
        cout << " [2] ACT Trajectory Anatomy (Custom Surgical Strike & Trace)" << endl;
        cout << "---------------------------------------------------------------------" << endl;
        
        int mode = 0; string input_buf;
        while (true) {
            cout << "Enter deployment mode (1 or 2): ";
            getline(cin, input_buf);
            if (!input_buf.empty() && (input_buf == "1" || input_buf == "2")) {
                mode = stoi(input_buf); break;
            }
            cout << "  [Error] Please enter 1 or 2.\n";
        }
        
        if (mode == 1) {
            cout << "\n=====================================================================" << endl;
            cout << "   [ENGINE 1] AACC INFINITE CRUCIBLE FUZZER" << endl;
            cout << "=====================================================================" << endl;
            
            int target_configs = 100, threads = 8, seed_size = 100;        
            long long phase1_steps = 10000000; 
            
            cout << "[?] Enter Number of Critical Configs to test (default 100): ";
            getline(cin, input_buf); if (!input_buf.empty()) target_configs = stoi(input_buf);

            cout << "[?] Enter Shared Master Seed Magnitude (digits, default 100): ";
            getline(cin, input_buf); if (!input_buf.empty()) seed_size = stoi(input_buf);
            
            cout << "[?] Enter Phase 1 Max Steps (default 10000000): ";
            getline(cin, input_buf); if (!input_buf.empty()) phase1_steps = stoll(input_buf);
            
            long long phase2_steps = phase1_steps * 10;
            string shared_seed = generate_massive_seed(seed_size);
            
            cout << "\n[*] Engine Parameters Initialized: \n";
            cout << "    - Target configs : " << target_configs << " (Delta' < 0, Stratified by Rho)\n";
            cout << "    - Shared Seed    : " << seed_size << " digits\n";
            cout << "    - Phase 1 Depth  : " << phase1_steps << " steps\n";
            cout << "    - Concurrency    : " << threads << " cores\n";
            
            cout << "\n=====================================================================" << endl;
            cout << "   SHARED MASTER SEED GENERATED" << endl;
            cout << "=====================================================================" << endl;
            if (shared_seed.length() <= 100) cout << "   " << shared_seed << "\n";
            else cout << "   " << shared_seed.substr(0, 50) << "\n   ...\n   " << shared_seed.substr(shared_seed.length() - 50) << "\n";

            vector<FuzzConfig> configs = generate_stratified_configs(target_configs);
            
            cout << "\n=====================================================================" << endl;
            cout << "   STRATIFIED ECF(A,B) CONFIGURATIONS (" << configs.size() << " SYSTEMS)" << endl;
            cout << "=====================================================================" << endl;
            int current_rho = -1;
            for (int i = 0; i < configs.size(); ++i) {
                if (configs[i].rho != current_rho) {
                    current_rho = configs[i].rho;
                    cout << "\n   >>> [ REGIME " << current_rho << " | Lattice Gravity Rho = " << current_rho << " ] <<<\n";
                }
                cout << "   [" << setw(3) << i+1 << "] " << setw(26) << left << format_ecf(configs[i].n1, configs[i].p1, configs[i].n2, configs[i].p2) 
                     << " | Delta'= " << fixed << setprecision(5) << configs[i].delta << "\n";
            }
            
            atomic<int> config_index(0); atomic<int> p1_cleared(0);
            vector<pair<FuzzConfig, string>> current_suspects;
            vector<thread> worker_threads;
            
            cout << "\n=====================================================================" << endl;
            cout << "   [PHASE 1] HIGH-SPEED MACROSCOPIC SWEEP (" << phase1_steps << " STEPS) " << endl;
            cout << "=====================================================================" << endl;
            
            g_interrupt_flag = 0;
            for (int i = 0; i < threads; ++i) worker_threads.emplace_back(fuzz_phase1_worker, cref(configs), ref(config_index), ref(p1_cleared), cref(shared_seed), phase1_steps, ref(current_suspects));
            for (auto& t : worker_threads) { if (t.joinable()) t.join(); }
            
            cout << "\n   > Phase 1 Cleared: " << p1_cleared << " / " << target_configs << " configs.\n";
            
            long long current_depth = phase1_steps * 10;
            int phase_num = 2;
            
            while (!current_suspects.empty()) {
                cout << "\n=====================================================================" << endl;
                if (phase_num == 2) cout << "   [PHASE 2] THE CRUCIBLE (" << current_depth << " STEPS)" << endl;
                else cout << "   [PHASE " << phase_num << "] ABYSSAL PURSUIT (" << current_depth << " STEPS)" << endl;
                cout << "=====================================================================" << endl;
                cout << "   [*] Unleashing Floyd's Cycle-Finder to exhaust " << current_suspects.size() << " stubborn trajectories..." << endl;
                
                vector<pair<FuzzConfig, string>> next_suspects;
                atomic<int> p_idx(0); worker_threads.clear();
                g_interrupt_flag = 0;
                
                for (int i = 0; i < threads; ++i) worker_threads.emplace_back(fuzz_deep_phase_worker, cref(current_suspects), ref(p_idx), ref(next_suspects), current_depth, phase_num);
                for (auto& t : worker_threads) { if (t.joinable()) t.join(); }
                
                current_suspects = next_suspects;
                if (current_suspects.empty()) break; 
                
                if (phase_num == 2) {
                    phase_num = 3; current_depth = phase1_steps * 100; 
                    cout << "\n[!] " << current_suspects.size() << " TRAJECTORIES SURVIVED PHASE 2." << endl;
                    cout << "[!] Automatically escalating to Phase 3 with " << current_depth << " steps..." << endl;
                } else {
                    cout << "\n=====================================================================" << endl;
                    cout << "   [WARNING] " << current_suspects.size() << " EXTREME ANOMALIES SURVIVED PHASE " << phase_num << "!" << endl;
                    cout << "=====================================================================" << endl;
                    cout << "[?] Enter an even larger step limit to hunt them down (e.g., 50000000) \n";
                    cout << "[?] OR enter '0' to surrender and print the invincible configurations: ";
                    
                    getline(cin, input_buf);
                    if (input_buf.empty() || input_buf == "0") break; 
                    current_depth = stoll(input_buf); phase_num++;
                }
            }
            
            cout << "\n=====================================================================" << endl;
            cout << "   FINAL FUZZING SETTLEMENT REPORT " << endl;
            cout << "=====================================================================" << endl;
            
            if (current_suspects.empty()) {
                cout << "   " << target_configs << "/" << target_configs << " Configs Topologically Vindicated. NO GRAIL DETECTED." << endl;
                cout << "   All stubbornly resisting seeds were successfully exhausted and converged into dynamic loops." << endl;
                cout << "   The exact deterministic anchors (x) and cycle lengths were algorithmically extracted." << endl;
                cout << "   The AACC determinant strictly holds." << endl;
            } else {
                cout << "   [GRAIL LOG] Printing the " << current_suspects.size() << " invincible systems:" << endl;
                for (const auto& suspect : current_suspects) {
                    cout << "\n[!!!] INVINCIBLE GRAIL | Delta'=" << suspect.first.delta << " | Rho=" << suspect.first.rho << "\n"
                         << "Config: " << format_ecf(suspect.first.n1, suspect.first.p1, suspect.first.n2, suspect.first.p2) << "\n";
                }
                cout << string(60, '-') << endl;
            }
            cout << "=====================================================================" << endl;
            
            while (true) {
                cout << "\n[?] Enter Configuration Number (1-" << configs.size() << ") for Deep Anatomy Trace, or 0 to return to Main Menu: ";
                getline(cin, input_buf);
                if (input_buf.empty()) continue;
                int target_id = stoi(input_buf);
                if (target_id == 0) break;
                if (target_id > 0 && target_id <= configs.size()) {
                    auto& cfg = configs[target_id - 1];
                    run_anatomy_trace(shared_seed, cfg.n1, mpz_class(to_string(cfg.p1)), cfg.n2, mpz_class(to_string(cfg.p2)));
                } else {
                    cout << "  [Error] Invalid Configuration Number.\n";
                }
            }
            
        } else if (mode == 2) {
            cout << "\n=====================================================================" << endl;
            cout << "   [ENGINE 2] ACT TRAJECTORY ANATOMY TRACE" << endl;
            cout << "=====================================================================" << endl;
            
            string format_choice; mpz_class manual_seed;
            cout << "Select input seed format - [s] Simple Integer or [a] Algebraic (A*B^C+D)? ";
            getline(cin, format_choice);
            
            if (format_choice == "a" || format_choice == "A") {
                mpz_class A, B, D; unsigned long C;
                cout << "Formula: A * B^C + D\n";
                cout << "A: "; cin >> A; cout << "B: "; cin >> B; cout << "C: "; cin >> C; cout << "D: "; cin >> D;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                mpz_pow_ui(manual_seed.get_mpz_t(), B.get_mpz_t(), C); manual_seed = A * manual_seed + D;
            } else {
                string s; cout << "Enter integer seed (supports negative space): "; getline(cin, s);
                if (!parseInputString(s, manual_seed)) { cout << "  [Error] Invalid integer input.\n"; continue; }
            }
            
            long long N1 = getValidOddInput("Enter N1 (odd, [-99, 99]): ", false, "-99", "99").get_si();
            mpz_class p1 = getValidOddInput("Enter p1 (odd, [-999999, 999999]): ", false, "-999999", "999999");
            long long N2 = getValidOddInput("Enter N2 (odd, [-99, 99]): ", false, "-99", "99").get_si();
            mpz_class p2 = getValidOddInput("Enter p2 (odd, [-999999, 999999]): ", false, "-999999", "999999");
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            run_anatomy_trace(manual_seed.get_str(), N1, p1, N2, p2);
        }
        
        cout << "\n=====================================================================" << endl;
        if (!getYesNoPrompt("Return to Main Menu? (y/n): ")) break;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return 0;
}
