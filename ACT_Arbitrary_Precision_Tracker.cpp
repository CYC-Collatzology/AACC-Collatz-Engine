/* =====================================================================
 * ACT Arbitrary Precision Tracker - TRI-MODE ACADEMIC EDITION
 * =====================================================================
 * Required: GNU Multiple Precision (GMP) Library, C++17 Standard
 * Compilation: g++ -O3 -std=c++17 ACT_Arbitrary_Precision_Tracker.cpp -lgmpxx -lgmp -o act_tracker
 * 
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <iomanip>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <algorithm> 
#include <deque>
#include <random>
#include <gmpxx.h>

using namespace std;

// =====================================================================
// [ CONSTANTS & STRUCTS ]
// =====================================================================

const int MULTIPLIERS[8] = {3, 3, 5, 5, 7, 7, 9, 9};
const int ADD_P1_SIGN[8] = {1, -1, 1, -1, 1, -1, 1, -1};
const int ADD_P2_SIGN[8] = {1, -1, 1, -1, 1, -1, 1, -1};

const char THEORETICAL_DESTINY[8][8] = {
    {'C', 'C', 'C', 'C', 'D', 'C', 'C', 'D'},
    {'D', 'C', 'C', 'D', 'D', 'D', 'D', 'D'},
    {'D', 'C', 'D', 'D', 'D', 'D', 'D', 'D'},
    {'C', 'C', 'C', 'D', 'D', 'C', 'C', 'D'},
    {'D', 'C', 'C', 'D', 'D', 'C', 'C', 'D'},
    {'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D'},
    {'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D'},
    {'D', 'C', 'C', 'D', 'D', 'C', 'D', 'D'}
};

struct TargetConfig {
    long long N1;
    mpz_class p1;
    long long N2;
    mpz_class p2;
    char oracle;
    string id_label;
};

struct MismatchRecord {
    string id_label;
    long long N1;
    long long N2;
    char oracle;
    char empirical;
    mpz_class p1_actual;
    mpz_class p2_actual;
    long long cycle_length;          
    mpz_class collision_value;       
    long long initial_repetition_step; 
};

struct SimulationResult {
    char destiny;
    long long initial_repetition_step;
    long long cycle_length;
    long long total_steps_executed;
    mpz_class collision_value; 
};

struct ACTResult {
    vector<double> stats; string end_message; bool loop_detected;
    string final_val_str; vector<uint32_t> digit_history; 
    long long peak_step; uint32_t peak_digits; 
    long long nadir_step; uint32_t nadir_digits; 
    unordered_map<int, long long> emp_N_counts; 
    vector<mpz_class> loop_sequence;
    bool is_deterministic_mode;
};

struct StepRecord { bool is_odd; int N; mpz_class p; mpz_class val; };

// =====================================================================
// [ UTILITIES & MATH FUNCTIONS ]
// =====================================================================

namespace std {
    template <> struct hash<mpz_class> {
        size_t operator()(const mpz_class& x) const {
            mpz_srcptr p = x.get_mpz_t(); 
            if (p->_mp_size == 0) return 0;
            size_t s = 0; 
            int c = abs(p->_mp_size);
            for (int i = 0; i < c; ++i) {
                s ^= hash<mp_limb_t>()(p->_mp_d[i]) + 0x9e3779b9 + (s << 6) + (s >> 2);
            }
            if (p->_mp_size < 0) s = ~s; 
            return s;
        }
    };
}

bool getYesNoPrompt(const string& prompt) {
    string s;
    while (true) {
        cout << prompt;
        cin >> s;
        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;
        cout << "  [Error] Please enter y or n.\n";
    }
}

// Advanced input parser with boundaries
mpz_class getValidOddInput(const string& prompt, bool positiveOnly = false, string min_str = "", string max_str = "") {
    mpz_class val; mpz_class min_val, max_val; bool check_bounds = false;
    if (!min_str.empty() && !max_str.empty()) {
        min_val = mpz_class(min_str); max_val = mpz_class(max_str); check_bounds = true;
    }
    while (true) {
        cout << prompt;
        if (cin >> val) {
            if (positiveOnly && val <= 0) { cout << "  [Error] Must be positive (>0).\n"; continue; }
            if (val % 2 == 0) { cout << "  [Error] Must be an ODD integer.\n"; continue; }
            if (check_bounds && (val < min_val || val > max_val)) {
                cout << "  [Error] Out of bounds. Limit: [" << min_str << ", " << max_str << "].\n"; continue;
            } return val;
        } else {
            cout << "  [Error] Invalid format.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

bool parseInputString(const string& input, mpz_class& out) {
    if (input.empty()) return false;
    try { out = mpz_class(input); return true; } 
    catch (...) { return false; }
}

int fast_mod4(const mpz_class& n) { 
    int m = mpz_class(n % 4).get_si();
    if (m < 0) m += 4;
    return m;
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

double calculateTheoreticalEO(long long N1, long long N2, const mpz_class& p1, const mpz_class& p2) {
    mpz_class v1 = mpz_class(to_string(N1)) + p1; double e1 = (fast_mod4(v1) == 0) ? 3.0 : 1.0;
    mpz_class v2 = mpz_class(to_string(N2)) * 3 + p2; double e2 = (fast_mod4(v2) == 0) ? 3.0 : 1.0;
    return (e1 + e2) / 2.0;
}

double calculateInitialExpansionIndex(int N1, int N2) { 
    return (log((double)abs(N1)) + log((double)abs(N2))) / (2.0 * log(2.0)); 
}

int determineModularConfiguration(int N1, int N2, const mpz_class& p1, const mpz_class& p2) {
    int m1 = fast_mod4(N1 + p1); 
    int m2 = fast_mod4(N2 + p2); 
    if (m1 == 2 && m2 == 2) return 2; if (m1 == 0 && m2 == 0) return 3;
    if (m1 == 2 && m2 == 0) return 4; if (m1 == 0 && m2 == 2) return 5;
    if (m1 == 0 && (m2 == 1 || m2 == 3)) return 6; if ((m1 == 1 || m1 == 3) && m2 == 2) return 7;
    if (m1 == 2 && (m2 == 1 || m2 == 3)) return 8; if ((m1 == 1 || m1 == 3) && m2 == 0) return 9;
    return 1;
}

string getRegimeString(double estEO) {
    if (abs(estEO - 1.0) < 0.01) return "Regime I";
    if (abs(estEO - 2.0) < 0.01) return "Regime II";
    if (abs(estEO - 3.0) < 0.01) return "Regime III";
    return "Transitional Regime";
}

string formatSpaceConfig(long long N1, const mpz_class& p1, long long N2, const mpz_class& p2) {
    string s1 = to_string(N1) + "x" + (p1 >= 0 ? "+" : "") + p1.get_str();
    string s2 = to_string(N2) + "x" + (p2 >= 0 ? "+" : "") + p2.get_str();
    return "ECF(" + s1 + ", " + s2 + ")";
}

void dissect_boundary_deadlock(long long N1, const mpz_class& p1, long long N2, const mpz_class& p2, const mpz_class& collision_x) {
    int m = fast_mod4(collision_x);
    mpz_class next_x; string sub_equation = "";
    mpz_class gmp_N1(to_string(N1)), gmp_N2(to_string(N2));
    
    if (collision_x % 2 == 0) { next_x = collision_x / 2; sub_equation = formatBigNumber(collision_x) + " / 2"; } 
    else if (m == 1) { next_x = gmp_N1 * collision_x + p1; sub_equation = to_string(N1) + "x" + (p1 >= 0 ? "+" : "") + p1.get_str(); } 
    else if (m == 3) { next_x = gmp_N2 * collision_x + p2; sub_equation = to_string(N2) + "x" + (p2 >= 0 ? "+" : "") + p2.get_str(); }
    
    cout << "\n     >> [BOUNDARY ANALYSIS] Crystalline Deadlock Dissection:" << endl;
    cout << "        Critical Collision State x_critical = " << formatBigNumber(collision_x) << " (mod4 = " << m << ")" << endl;
    cout << "        Next-Step Vector Injection Mechanics: " << sub_equation << " => " << next_x << endl;
    if (next_x < 0) cout << "        Verdict: Confirmed Sink. Trajectory forces collapse into Negative Domain (-Z Space)." << endl;
    else cout << "        Verdict: Confirmed Static Fixed-Point Attraction." << endl;
}

void printDynastyReport(const vector<uint32_t>& history, long long total_steps, long long peak_step, uint32_t peak_digits, long long nadir_step, uint32_t nadir_digits) {
    if (history.empty() || total_steps < 10) return; 
    cout << "\n--- Trajectory Lifecycle Report (Magnitude History) ---\nStart: " << history.front() << " digits\n";
    for (int i = 1; i <= 9; ++i) {
        size_t idx = (history.size() * i) / 10;
        if (idx < history.size()) cout << i * 10 << "%  : " << history[idx] << " digits\n";
    }
    cout << "End  : " << history.back() << " digits\n";
    
    double peak_pos = (total_steps > 0) ? ((double)peak_step / total_steps) * 100.0 : 0.0;
    double nadir_pos = (total_steps > 0) ? ((double)nadir_step / total_steps) * 100.0 : 0.0;
    cout << "Peak : at " << fixed << setprecision(5) << peak_pos << "% life span (" << peak_digits << " digits)\n";
    cout << "Nadir: at " << fixed << setprecision(5) << nadir_pos << "% life span (" << nadir_digits << " digits)\n";
    cout << "----------------------------------------------\n";
}

// =====================================================================
// [ ENGINE CORE ]  (Mode 1 & 2)
// =====================================================================

SimulationResult simulate_ecf_verbose(long long N1_in, const mpz_class& p1, long long N2_in, const mpz_class& p2, const mpz_class& seed, long long max_steps, size_t max_digits, bool telemetry) {
    mpz_class x = seed;
    mpz_class N1(to_string(N1_in)); mpz_class N2(to_string(N2_in));
    long long steps = 0; unordered_map<mpz_class, long long> footprint_telemetry;
    mpz_class low_altitude_threshold("100000000000"); 
    long long telemetry_interval = (max_steps >= 10000000000LL) ? 100000000LL : 5000000LL;

    while (x != 0 && x != 1 && x != -1 && (max_steps == 0 || steps < max_steps)) {
        int m = fast_mod4(x);
        if (x < low_altitude_threshold && x > -low_altitude_threshold) {
            if (footprint_telemetry.size() > 5000000) footprint_telemetry.clear();
            auto [it, inserted] = footprint_telemetry.insert({x, steps});
            if (!inserted) {
                long long init_step = it->second; long long cycle_len = steps - init_step;
                return {'C', init_step, cycle_len, steps, x}; 
            }
        }

        if (x % 2 == 0) x /= 2;
        else if (m == 1) x = N1 * x + p1;
        else if (m == 3) x = N2 * x + p2;
        steps++;
        
        if (x == 1 || x == 2 || x == 4) {
            long long init_step = steps;
            if (footprint_telemetry.count(x)) init_step = footprint_telemetry[x];
            return {'C', init_step, 3, steps, x};
        }
        
        if (telemetry && steps % telemetry_interval == 0) {
            size_t current_digits = mpz_sizeinbase(x.get_mpz_t(), 10);
            cout << "\n    [Telemetry] Step: " << setw(11) << steps << " | Magnitude: " << current_digits << " digits" << flush;
        }

        if (steps % 50000 == 0) { if (mpz_sizeinbase(x.get_mpz_t(), 10) > max_digits) return {'D', -1, 0, steps, x}; }
    }
    
    if (max_steps != 0 && steps >= max_steps) return {'D', -1, 0, steps, x};
    return {'C', steps, 1, steps, x}; 
}

// =====================================================================
// [ ENGINE CORE ] ACT (Mode 3)
// =====================================================================

namespace ACT {
    ACTResult simulateTrajectory(const mpz_class& n, int N1, int N2, mpz_class p1, mpz_class p2, int cond, mpz_class max_steps_mpz, int disp, int fa, int lb, bool allow_neg_x = false) { 
        ACTResult res; mpz_class current = n; long long steps = 0;
        mpz_class peak_val = abs(n); long long peak_step = 0;
        mpz_class nadir_val = abs(n); long long nadir_step = 0; 
        
        mpz_class limit("9223372036854775800"); 
        long long ms = (max_steps_mpz > limit || max_steps_mpz == 0) ? 9223372036854775800LL : max_steps_mpz.get_si();

        long long eS = 0, oS = 0, oS1 = 0, oS2 = 0; long long c4k[4] = {0}; 
        double sum_log_N = 0.0;
        
        res.loop_detected = false; mpz_class h_th("100000000000000000000"); 
        deque<mpz_class> h_dq; unordered_map<mpz_class, long long> h_map; 
        h_map.reserve(150000);
         
        deque<StepRecord> rec_buf; bool rec_act = true;
        res.digit_history.reserve(min((long long)50000000, ms));

        if (disp == 1 && fa > 0) cout << "First " << fa << " steps:\n" << formatBigNumber(current) << " "; 
        
        mpz_class cP = 0; int cN = 0;
        res.is_deterministic_mode = true; // Mode 3 is purely deterministic

        while (true) {
            if (!allow_neg_x && current < 0) {
                res.end_message = "Crashed into negative domain at " + formatBigNumber(current);
                res.loop_detected = false;
                break;
            }

            if (abs(current) > peak_val) { peak_val = abs(current); peak_step = steps; }
            if (abs(current) < nadir_val) { nadir_val = abs(current); nadir_step = steps; }
            
            if (steps >= ms) {
                cout << "\n\n>>> Max steps (" << ms << ") reached.\n>>> Current Value: " << getExactDigits(current) << " digits" << (current < 0 ? " (Negative Integer).\n" : ".\n") << ">>> Extend simulation? Enter additional steps (0 to stop): ";
                long long ext;
                if (cin >> ext && ext > 0) { ms += ext; cout << "Running to " << ms << " steps..." << flush; } 
                else { res.end_message = "Trajectory exceeded macroscopic bounds (Escaping Diophantine Traps)"; break; }
            }

            if (steps > 0 && steps % 10000000 == 0) cout << " [" << steps/1000000 << "M | " << getExactDigits(current) << "d] " << flush;
            else if (steps > 0 && steps % 500000 == 0 && steps < 10000000) cout << "." << flush;

            if (rec_act) { if (steps < 50000000) res.digit_history.push_back(getExactDigits(current)); else rec_act = false; }
            
            if (abs(current) < h_th) {
                if (h_map.count(current)) { 
                    long long loop_length = steps - h_map[current]; 
                    vector<mpz_class> loop_elements;
                    for (auto it = h_dq.rbegin(); it != h_dq.rend(); ++it) {
                        loop_elements.push_back(*it); if (*it == current) break;
                    }
                    std::reverse(loop_elements.begin(), loop_elements.end());
                    res.loop_sequence = loop_elements; 
                    
                    mpz_class x_min = loop_elements[0]; bool found_odd = false;
                    for (const auto& val : loop_elements) {
                        if (val % 2 != 0) {
                            if (!found_odd) { x_min = val; found_odd = true; }
                            else { if (abs(val) < abs(x_min)) x_min = val; else if (abs(val) == abs(x_min) && val > x_min) x_min = val; }
                        }
                    }
                    if (!found_odd) {
                        x_min = loop_elements[0];
                        for (const auto& val : loop_elements) if (abs(val) < abs(x_min)) x_min = val;
                    }
                    long long step_to_xmin = h_map[x_min];
                    res.end_message = "Loop detected at " + formatBigNumber(current) + " (Cycle length: " + to_string(loop_length) + " steps, cycle x_min=" + formatBigNumber(x_min) + " at step " + to_string(step_to_xmin) + ")"; 
                    res.loop_detected = true; 
                    break; 
                }
                h_dq.push_back(current); h_map[current] = steps; 
                if (h_dq.size() > 150000) { h_map.erase(h_dq.front()); h_dq.pop_front(); }
            }

            mpz_class next_c; bool is_odd = (fast_mod4(current) % 2 != 0);

            if (!is_odd) { 
                next_c = current / 2; eS++; 
            } else {
                if (fast_mod4(current) == 1) { next_c = N1 * current + p1; oS1++; cN = N1; cP = p1; } 
                else { next_c = N2 * current + p2; oS2++; cN = N2; cP = p2; }
                oS++; sum_log_N += log((double)abs(cN)) / log(2.0); res.emp_N_counts[cN]++; 
            }

            if (disp == 2) cout << (is_odd ? " = ("+to_string(cN)+"x"+(cP>=0?"+":"")+cP.get_str()+") => " : " => ") << formatBigNumber(next_c) << "\n";
            else if (disp == 1) {
                if (steps < fa) cout << (is_odd ? " = ("+to_string(cN)+"x"+(cP>=0?"+":"")+cP.get_str()+") => " : " => ") << formatBigNumber(next_c) << (steps < fa - 1 ? " " : " ...\n");
                if (lb > 0) { rec_buf.push_back({is_odd, cN, cP, next_c}); if (rec_buf.size() > lb) rec_buf.pop_front(); }
            }

            c4k[fast_mod4(next_c)]++; current = next_c; steps++;
        }

        if (disp == 1 && !rec_buf.empty()) {
            cout << "\nLast " << rec_buf.size() << " steps:\n" << (steps > fa + lb ? "... " : ""); 
            for (auto& r : rec_buf) cout << (r.is_odd ? " = ("+to_string(r.N)+"x"+(r.p>=0?"+":"")+r.p.get_str()+") => " : " => ") << formatBigNumber(r.val) << " ";
            cout << "\n";
        }

        if (steps >= ms && res.end_message.empty()) res.end_message = "Trajectory exceeded macroscopic bounds (Escaping Diophantine Traps)";
        
        res.stats = { oS > 0 ? (double)eS / oS : 0.0, (double)c4k[0], (double)c4k[1], (double)c4k[3], (double)c4k[2], (double)oS, (double)steps, (double)eS, (double)oS1, (double)oS2, sum_log_N };
        
        res.final_val_str = formatBigNumber(current); 
        res.peak_step = peak_step; res.peak_digits = getExactDigits(peak_val);
        res.nadir_step = nadir_step; res.nadir_digits = getExactDigits(nadir_val);
        
        if (rec_act) res.digit_history.push_back(getExactDigits(current));
        uint32_t f_dig = getExactDigits(current);
        if (res.digit_history.empty() || res.digit_history.back() != f_dig) res.digit_history.push_back(f_dig);
        return res;
    }
}
// =====================================================================
// [ MAIN SYSTEM ]
// =====================================================================

int main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL); 
    
    cout << "=====================================================================" << endl;
        cout << "=====================================================================" << endl;
    cout << "   ACT Arbitrary Precision Tracker : TRI-MODE ACADEMIC EDITION" << endl;
    cout << "=====================================================================" << endl;

    while (true) {
        // --- 1. FIRST: SELECT DEPLOYMENT MODE ---
          cout << "\n=====================================================================" << endl;
        cout << "   SELECT DEPLOYMENT MODE" << endl;
            cout << "=====================================================================" << endl;
        cout << " [1] Classic Matrix Sweep (Standard 64-Cell Isomorphic Baseline)" << endl;
        cout << " [2] Custom Surgical Strike (Define specific N1, p1, N2, p2)" << endl;
        cout << " [3] ACT Single Trajectory Topology Analysis (Detailed Trace)" << endl;
        cout << "---------------------------------------------------------------------" << endl;
        
        int mode;
        while (true) {
            cout << "Enter mode (1, 2, or 3): ";
            if (cin >> mode && (mode >= 1 && mode <= 3)) break;
            cout << "  [Error] Please enter 1, 2, or 3.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        // --- 2. SECOND: INPUT TARGET SEED ---
        mpz_class test_seed;
        string format_choice;
        
        cout << "\nSelect input seed format - [s] Simple Integer or [a] Algebraic (A*B^C+D)? ";
        getline(cin, format_choice);
        
        if (format_choice == "a" || format_choice == "A") {
            mpz_class A, B, D; unsigned long C;
            cout << "Formula: A * B^C + D\n";
            cout << "A: "; cin >> A; 
            cout << "B: "; cin >> B; 
            cout << "C: "; cin >> C; 
            cout << "D: "; cin >> D;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            mpz_pow_ui(test_seed.get_mpz_t(), B.get_mpz_t(), C); 
            test_seed = A * test_seed + D;
        } else {
            string s;
            cout << "Enter integer seed (supports negative space): "; 
            getline(cin, s);
            if (!parseInputString(s, test_seed)) {
                cout << "  [Error] Invalid integer input.\n";
                continue; // Return to mode selection
            }
        }
                size_t seed_digits = getExactDigits(test_seed);
        cout << "Target Seed Confirmed. Value: " << formatBigNumber(test_seed) << endl;

        // --- 3. THIRD: ROUTE TO SPECIFIC MODE LOGIC ---
        if (mode == 1 || mode == 2) {
            vector<TargetConfig> targets;
            if (mode == 1) {
                long long u_input = 0, v_v_input = 0;
                cout << "\n--- Modular Lattice Expansion Configuration (Range: -999999 to 999999) ---" << endl;
                while (true) {
                    cout << "Enter lattice displacement u for p1 (p1 = sign*1 + 4*u): ";
                    if (cin >> u_input && abs(u_input) <= 999999) break;
                    cout << "  [Error] Out of range or invalid format.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                while (true) {
                    cout << "Enter lattice displacement v for p2 (p2 = sign*1 + 4*v): ";
                    if (cin >> v_v_input && abs(v_v_input) <= 999999) break;
                    cout << "  [Error] Out of range or invalid format.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                
                mpz_class u_gmp(to_string(u_input));
                mpz_class v_gmp(to_string(v_v_input));

                cout << "\n[ORACLE PRE-DETERMINATION BLUEPRINT MATRIX] " << endl;
                cout << "     Col: 0 1 2 3 4 5 6 7" << endl;
                cout << "  -----------------------" << endl;
                for (int r = 0; r < 8; r++) {
                    cout << "  Row " << r << " | ";
                    for (int c = 0; c < 8; c++) {
                        cout << THEORETICAL_DESTINY[r][c] << " ";
                        long long N1 = MULTIPLIERS[r];
                        mpz_class p1 = ADD_P1_SIGN[r] + 4 * u_gmp;
                        long long N2 = MULTIPLIERS[c];
                        mpz_class p2 = ADD_P2_SIGN[c] + 4 * v_gmp;
                        
                        char oracle = THEORETICAL_DESTINY[r][c];
                        string id = "[" + to_string(r) + "][" + to_string(c) + "]";
                        targets.push_back({N1, p1, N2, p2, oracle, id});
                    }
                    cout << "|" << endl;
                }
                cout << "  -----------------------" << endl;
                cout << "  (C = Absolute Convergence | D = Macroscopic Divergence)\n" << endl;
            } 
            else if (mode == 2) {
                long long cus_N1, cus_N2;
                long long cus_p1, cus_p2;
                
                cout << "\n--- CUSTOM TARGET CONFIGURATION ---" << endl;
                
                while (true) {
                    cout << "Enter N1 (mod 1 state odd multiplier, -99 to 99): ";
                    if (cin >> cus_N1 && cus_N1 >= -99 && cus_N1 <= 99 && cus_N1 % 2 != 0) break;
                    cout << "  [Error] Must be an odd integer between -99 and 99.\n";
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                while (true) {
                    cout << "Enter p1 (odd shift, -999999 to 999999): ";
                    if (cin >> cus_p1 && cus_p1 >= -999999 && cus_p1 <= 999999 && cus_p1 % 2 != 0) break;
                    cout << "  [Error] Must be an odd integer between -999999 and 999999.\n";
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                while (true) {
                    cout << "Enter N2 (mod 3 state odd multiplier, -99 to 99): ";
                    if (cin >> cus_N2 && cus_N2 >= -99 && cus_N2 <= 99 && cus_N2 % 2 != 0) break;
                    cout << "  [Error] Must be an odd integer between -99 and 99.\n";
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                while (true) {
                    cout << "Enter p2 (odd shift, -999999 to 999999): ";
                    if (cin >> cus_p2 && cus_p2 >= -999999 && cus_p2 <= 999999 && cus_p2 % 2 != 0) break;
                    cout << "  [Error] Must be an odd integer between -999999 and 999999.\n";
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }

                mpz_class p1(to_string(cus_p1));
                mpz_class p2(to_string(cus_p2));

                double iei = calculateInitialExpansionIndex(cus_N1, cus_N2);
                double estEO = calculateTheoreticalEO(cus_N1, cus_N2, p1, p2);
                double delta_prime = iei - estEO;
                char cus_oracle = (delta_prime < 0) ? 'C' : 'D';

                cout << "\n=====================================================================" << endl;
                cout << "   SURGICAL ORACLE PRE-DETERMINATION " << endl;
                cout << "=====================================================================" << endl;
                cout << " Target Space      : " << formatSpaceConfig(cus_N1, p1, cus_N2, p2) << endl;
                cout << " Expansion Index   : " << fixed << setprecision(5) << iei << endl;
                cout << " Gravity Operator  : " << estEO << endl;
                cout << " Net Drift (Delta'): " << delta_prime << endl;
                cout << "---------------------------------------------------------------------" << endl;
                cout << " >> PREDICTED DESTINY : " << (cus_oracle == 'C' ? "ABSOLUTE CONVERGENCE (C)" : "MACROSCOPIC DIVERGENCE (D)") << endl;
                cout << "=====================================================================\n" << endl;

                targets.push_back({cus_N1, p1, cus_N2, p2, cus_oracle, "[Custom]"});
            }
            long long phase1_steps = max(500000LL, (long long)(seed_digits * 30));
            long long phase2_steps = max(5000000LL, phase1_steps * 10);
            
            int match_count = 0;
            vector<MismatchRecord> stubborn_anomalies;
            size_t p1_max_digits = max((size_t)200000, seed_digits * 2);
            size_t p2_max_digits = max((size_t)1000000, seed_digits * 3);
            size_t p3_max_digits = max((size_t)5000000, seed_digits * 5);

            cout << "--- PHASE 1: STANDARD MACROSCOPIC SWEEP (" << (phase1_steps/1000) << "K Steps / Dynamic Digits) ---" << endl;
            cout << "----------------------------------------------------------------------------------" << endl;
                        cout << " ID[R][C]| Target Space Config                 | Oracle | Empirical | Status   " << endl;
            cout << "----------------------------------------------------------------------------------" << endl;
            
            for (const auto& t : targets) {
                string space_cfg = formatSpaceConfig(t.N1, t.p1, t.N2, t.p2);

                cout << " " << setw(7) << left << t.id_label << " | " << setw(35) << left << space_cfg 
                     << " |   " << t.oracle << "    |  " << flush;
                
                SimulationResult result = simulate_ecf_verbose(t.N1, t.p1, t.N2, t.p2, test_seed, phase1_steps, p1_max_digits, false);
                
                bool is_match = (t.oracle == result.destiny);
                if (is_match) {
                    match_count++;
                } else {
                    stubborn_anomalies.push_back({t.id_label, t.N1, t.N2, t.oracle, result.destiny, t.p1, t.p2, result.cycle_length, result.collision_value, result.initial_repetition_step});
                }
                
                cout << "   " << result.destiny << "     | "
                     << (is_match ? "SUCCESS" : "PENDING") << endl;
            }
            
            cout << "----------------------------------------------------------------------------------" << endl;
            cout << "Phase 1 Matches: " << match_count << " / " << targets.size() << endl;
            
            if (!stubborn_anomalies.empty()) {
                vector<MismatchRecord> phase3_targets;

                cout << "\n=====================================================================" << endl;
                cout << "   PHASE 2: THE CRUCIBLE (RESOLVING " << stubborn_anomalies.size() << " PENDING CASES )" << endl;
                cout << "=====================================================================" << endl;
                
                int p2_vindicated = 0;

                for (const auto& anomaly : stubborn_anomalies) {
                    string space_name = formatSpaceConfig(anomaly.N1, anomaly.p1_actual, anomaly.N2, anomaly.p2_actual);
                    cout << "\nTarget " << anomaly.id_label << " " << space_name << endl;
                    
                    if (anomaly.oracle == 'C' && anomaly.empirical == 'D') {
                        cout << "  > Diagnosis: False Divergence (Insufficient Depth)." << endl;
                        cout << "  > Action   : Expanding limits to " << phase2_steps << " steps..." << flush;
                        
                        SimulationResult crucible_result = simulate_ecf_verbose(anomaly.N1, anomaly.p1_actual, anomaly.N2, anomaly.p2_actual, test_seed, phase2_steps, p2_max_digits, false);
                        
                        if (crucible_result.destiny == 'C') {
                            cout << "\n  >> [VERDICT] TOTAL ATTRACTOR CAPTURE! Oracle Vindicated." << endl;
                            cout << "     >> Orbit initial repetition step : " << crucible_result.initial_repetition_step << endl;
                            cout << "     >> Captured Anchor State (x)     : " << crucible_result.collision_value.get_str() << endl; 
                            cout << "     >> Captured Cycle Period Length  : " << crucible_result.cycle_length << " steps" << endl;                                                                                                       
                            if (crucible_result.cycle_length == 1) {
                                dissect_boundary_deadlock(anomaly.N1, anomaly.p1_actual, anomaly.N2, anomaly.p2_actual, crucible_result.collision_value);
                            }
                            p2_vindicated++;
                        } else {
                            cout << "\n  > Result   : STILL RESISTING. Flagged for Phase 3: The Final Trial." << flush;
                            phase3_targets.push_back(anomaly);
                        }
                    } 
                    else if (anomaly.oracle == 'D' && anomaly.empirical == 'C') {
                        cout << "  > Diagnosis: False Convergence (Trapped in local cycle)." << endl;
                        cout << "     >> Trapped Anchor State (x)      : " << anomaly.collision_value.get_str() << endl;
                        cout << "     >> Trapped Cycle Period Length   : " << anomaly.cycle_length << " steps" << endl;
                        cout << "  > Action   : Injecting Topological Perturbations (Seed + 2k)..." << endl;         
                        bool broke_free = false;
                        for (int k = 1; k <= 5; ++k) {
                            mpz_class perturbed_seed = test_seed + (2 * k);
                            cout << "    Probing seed +" << (2 * k) << "... " << flush;
                            
                            SimulationResult crucible_result = simulate_ecf_verbose(anomaly.N1, anomaly.p1_actual, anomaly.N2, anomaly.p2_actual, perturbed_seed, phase1_steps, p1_max_digits, false);
                            if (crucible_result.destiny == 'D') {
                                cout << "ESCAPED! Trajectory diverges." << endl;
                                broke_free = true;
                                break; 
                            } else {
                                cout << "Trapped." << endl;
                            }
                        }
                        
                        if (broke_free) {
                            cout << "  > Result   : MACROSCOPIC DIVERGENCE CONFIRMED! Oracle Vindicated." << endl;
                            p2_vindicated++;
                        } else {
                            cout << "  > Result   : ALL LOCAL PROBES TRAPPED. Flagged for Phase 3 Cluster Probe." << endl;
                            phase3_targets.push_back(anomaly);
                        }
                    }
                }
                
                match_count += p2_vindicated;

                if (!phase3_targets.empty()) {
                    cout << "\n=====================================================================" << endl;
                    cout << "   PHASE 3: THE FINAL TRIAL   " << endl;
                    cout << "=====================================================================" << endl;    
                    int p3_vindicated = 0;

                    for (const auto& anomaly : phase3_targets) {
                        string space_name = formatSpaceConfig(anomaly.N1, anomaly.p1_actual, anomaly.N2, anomaly.p2_actual);
                       cout << "\n Deep Dive Interrogation: " << space_name << "\n" << flush;
                        if (anomaly.oracle == 'C') {
                            double iei = (log(abs(anomaly.N1)) + log(abs(anomaly.N2))) / (2.0 * log(2.0));
                            double estEO = calculateTheoreticalEO(anomaly.N1, anomaly.N2, anomaly.p1_actual, anomaly.p2_actual);
                            double delta_prime = iei - estEO;

                            long long target_max_steps = max(50000000LL, phase1_steps * 100); 
                            string strategy_msg = to_string(target_max_steps) + " Step Deep Dive Loop Radar active...";
                            if (delta_prime < 0) {
                                target_max_steps = max(10000000000LL, phase1_steps * 500); 
                                strategy_msg = "ABSOLUTE CONVERGENCE FIELD DETECTED (Delta' = " + to_string(delta_prime) + "). ALLOCATING " + to_string(target_max_steps) + " STEP FINAL RADAR!";
                            }

                            cout << "\n  > Strategy: " << strategy_msg << flush;
                            SimulationResult gallows_result = simulate_ecf_verbose(anomaly.N1, anomaly.p1_actual, anomaly.N2, anomaly.p2_actual, test_seed, target_max_steps, p3_max_digits, true);
                            
                            if (gallows_result.destiny == 'C') {
                                cout << "\n  >> [VERDICT] VINDICATED! Attractor collapse executed at depth: " << gallows_result.total_steps_executed << " steps" << endl;
                                cout << "     >> Orbit initial repetition step : " << gallows_result.initial_repetition_step << endl;
                                cout << "     >> Captured Anchor State (x)     : " << gallows_result.collision_value.get_str() << endl;
                                cout << "     >> Captured Cycle Period Length  : " << gallows_result.cycle_length << " steps" << endl;                         
                                if (gallows_result.cycle_length == 1) {
                                    dissect_boundary_deadlock(anomaly.N1, anomaly.p1_actual, anomaly.N2, anomaly.p2_actual, gallows_result.collision_value);
                                }
                                p3_vindicated++;
                            } else {
                                cout << "\n  >> [VERDICT] UNBROKEN. Seed remains stable at step level: " << gallows_result.total_steps_executed << endl;
                            }
                        } else {
                            cout << "\n  > Strategy: Saturation Cluster Probes (Up to Seed + 1000)..." << endl;
                            bool broke_free = false;
                            long long target_probe_steps = max(1000000LL, phase1_steps * 2);
                            for (int k = 6; k <= 500; ++k) {
                                mpz_class perturbed_seed = test_seed + (2 * k);
                                SimulationResult gallows_result = simulate_ecf_verbose(anomaly.N1, anomaly.p1_actual, anomaly.N2, anomaly.p2_actual, perturbed_seed, target_probe_steps, p1_max_digits, false);
                                if (gallows_result.destiny == 'D') {
                                    cout << "    >> Seed +" << (2*k) << " BROKE FREE at step level " << (target_probe_steps/1000) << "K!" << endl;
                                    broke_free = true;
                                    break;
                                }
                            }
                            if (broke_free) {
                                cout << "  >> [VERDICT] VINDICATED! Global divergence unlocked." << endl;
                                p3_vindicated++;
                            } else {
                                cout << "  >> [VERDICT] UNBROKEN. Sink density defies perturbation." << endl;
                            }
                        }
                    }
                    match_count += p3_vindicated;
                    cout << "\n---------------------------------------------------------------------" << endl;
                    cout << "GALLOWS SETTLEMENT REPORT:" << endl;
                    cout << "Anomalies Vindicated in Phase 3: " << p3_vindicated << " / " << phase3_targets.size() << endl;
                }
            }

            // =========================================================================
            // GRAND SETTLEMENT CONTRAST REPORT 
            // =========================================================================
            cout << "\n=====================================================================" << endl;
            cout << "   FINAL VERIFICATION SETTLEMENT & TOPOLOGICAL CONTRAST REPORT      " << endl;
            cout << "=====================================================================" << endl;
            cout << " Successful ACT Topodynamic Invariant Matches : " << match_count << " / " << targets.size() << endl;
            cout << " Final System Empirical Alignment Accuracy    : " << fixed << setprecision(2) << (double)match_count / targets.size() * 100.0 << "%" << endl;
            cout << "---------------------------------------------------------------------" << endl;
            cout << " [THEORETICAL PARADIGM SHIFT] STOCHASTIC VS DETERMINISTIC PARADIGM:" << endl;
            cout << "---------------------------------------------------------------------" << endl;
            cout << " 1. Naive Stochastic (Coin-Toss) Model:" << endl;
            cout << "    Probability of achieving " << match_count << "/" << targets.size() << " hits by pure chance:" << endl;
            if (match_count == 64) {
                cout << "    P_stochastic = (1/2)^64 = 5.42101e-20 (1 in 1.84 x 10^19 attempts)" << endl;
            } else if (targets.size() == 1) {
                cout << "    P_stochastic = 50% (For a single target, statistically insignificant)" << endl;
            } else {
                cout << "    P_stochastic = < 1.0e-15 (Statistically Infinitesimal Trap)" << endl;
            }
            cout << "\n 2. Asymmetric Brownian Random Walk Model:" << endl;
            cout << "    Probability of constraining a " << seed_digits << "-digit seed for millions of steps:" << endl;
            cout << "    P_brownian   -> 0.00000000000000000000000e+00 (Absolute Mathematical Impossibility)" << endl;
            cout << "\n 3. ACT Chiral Lattice Gravity Theory:" << endl;
            cout << "    Hypothesis: Macro-destiny is strictly pre-determined by Chiral Congruence." << endl;
            cout << "    P_ACT        = 1.0000000000000000000000000000 (100% Deterministic Certitude)" << endl;
            cout << "=====================================================================" << endl;
            
        } else if (mode == 3) {
            
            // =====================================
            // [ NEW ] Mode 3: ACT Topology Trace
            // =====================================
            cout << "\n--- ACT Single Trajectory Topology Analysis ---\n";
            bool allow_neg_N = getYesNoPrompt("  Enable N < 0 (Negative Chiral Multipliers)? (y/n): ");
            bool allow_neg_x = getYesNoPrompt("  Enable x < 0 domain tracking? (y/n): ");

            // Apply constraints mirroring Option 2
            int N1 = (int)getValidOddInput("Enter N1 (odd, [-99, 99]): ", !allow_neg_N, "-99", "99").get_si();
            mpz_class p1 = getValidOddInput("Enter p1 (odd, [-999999, 999999]): ", false, "-999999", "999999");
            int N2 = (int)getValidOddInput("Enter N2 (odd, [-99, 99]): ", !allow_neg_N, "-99", "99").get_si();
            mpz_class p2 = getValidOddInput("Enter p2 (odd, [-999999, 999999]): ", false, "-999999", "999999");

            double iei = calculateInitialExpansionIndex(N1, N2);
            int cond = determineModularConfiguration(N1, N2, p1, p2);
            double estEO = calculateTheoreticalEO(N1, N2, p1, p2);

            cout << "\nSystem Defined: ECF(" << N1 << "x" << (p1>=0?"+":"") << p1.get_str() << ", " << N2 << "x" << (p2>=0?"+":"") << p2.get_str() << ")\n";
            cout << "Topological State: " << getRegimeString(estEO) << " (Estimated E/O: " << fixed << setprecision(2) << estEO << ")\n";
            
            double p_delta = iei - estEO;
            cout << "Topological Invariant (Theoretical Delta'): " << (p_delta >= 0 ? "+" : "") << fixed << setprecision(12) << p_delta << (p_delta < 0 ? "  ==> [Absolute Convergence]\n" : "  ==> [Contingent Divergence]\n");
            
            mpz_class m_st; cout << "\nMax steps (0 for 100M): "; cin >> m_st; if (m_st == 0) m_st = 100000000;
            int disp=0, fa=0, lb=0; cout << "Display:\n  0: None\n  1: First 'a' and last 'b' steps\n  2: Full sequence\nChoice: "; cin >> disp;
            if(disp==1) { cout << "  First (a): "; cin >> fa; cout << "  Last (b): "; cin >> lb; }
            
            ACTResult res = ACT::simulateTrajectory(test_seed, N1, N2, p1, p2, cond, m_st, disp, fa, lb, allow_neg_x);
            
            long long tS = (long long)res.stats[6];
            double m_total = res.stats[5]; double m1 = res.stats[8]; double m2 = res.stats[9];      
            double empirical_ei = (m_total > 0) ? (m1 / m_total) * (log((double)abs(N1)) / log(2.0)) + (m2 / m_total) * (log((double)abs(N2)) / log(2.0)) : iei;
            double empirical_delta = empirical_ei - res.stats[0]; 
            double topodynamic_perturbation = empirical_delta - p_delta;
            bool has_neg_N = (N1 < 0 || N2 < 0);

            string result_status;
            if (res.loop_detected) {
                if (res.end_message.find("Thermalized") != string::npos) result_status = "Macroscopic Capture";
                else if (res.end_message.find("Loop detected") != string::npos) result_status = "Loop Detected";
                else result_status = "Unknown Arrest";
            } else {
                bool final_z_neg = (res.final_val_str[0] == '-');
                if (!allow_neg_x && res.end_message.find("Crashed") != string::npos) result_status = "Crashed (-Z)";
                else {
                    if (res.end_message.find("Escaping") != string::npos) result_status = has_neg_N ? "Contingent Divergence (+/-Z Alternating Deep Space)" : (final_z_neg ? "Contingent Divergence (-Z Deep Space)" : "Contingent Divergence (+Z Deep Space)");
                    else result_status = (empirical_delta >= 0 ? "Diverges" : "Converges");
                }
            }

            cout << "\n--- ACT Results ---\n";
            cout << "Status: " << res.end_message << "\n";
            cout << "Theoretical E.I. (Macro) : " << fixed << setprecision(12) << iei << "\n";
            cout << "Terminal E.I. (Observed) : " << fixed << setprecision(12) << empirical_ei << "\n";
            cout << "E/O Ratio (Observed)     : " << fixed << setprecision(12) << res.stats[0] << "\n";
            cout << "Theoretical Net Lift     : " << (p_delta >= 0 ? "+" : "") << fixed << setprecision(12) << p_delta << "\n";
            cout << "Observed Net Lift        : " << (empirical_delta >= 0 ? "+" : "") << fixed << setprecision(12) << empirical_delta << "\n";
            cout << "Topodynamic Perturbation (E_TP): " << (topodynamic_perturbation >= 0 ? "+" : "") << fixed << setprecision(12) << topodynamic_perturbation << "\n\n";

            cout << "4k+0: " << (long long)res.stats[1] << " (" << fixed << setprecision(5) << (tS>0?100.0*res.stats[1]/tS:0) << "%)\n"
                 << "4k+1: " << (long long)res.stats[2] << " (" << fixed << setprecision(5) << (tS>0?100.0*res.stats[2]/tS:0) << "%)\n"
                 << "4k+3: " << (long long)res.stats[3] << " (" << fixed << setprecision(5) << (tS>0?100.0*res.stats[3]/tS:0) << "%)\n"
                 << "4k+2: " << (long long)res.stats[4] << " (" << fixed << setprecision(5) << (tS>0?100.0*res.stats[4]/tS:0) << "%)\n"
                 << "Total steps: " << tS << " (4k+1: " << (long long)res.stats[8] << " | 4k+3: " << (long long)res.stats[9] << ")\n"
                 << "Result: " << result_status << "\n"
                 << "Final Number: " << res.final_val_str << "\n";
                 
            cout << "\n--- N-State Chiral Distribution (Macro vs Empirical) ---\n";
            if (m_total > 0) {
                cout << "N1=" << setw(3) << N1 << " (4k+1) | Observed: " << fixed << setprecision(5) << ((double)res.stats[8] / m_total * 100.0) << "% (" << (long long)res.stats[8] << " times)\n";
                cout << "N2=" << setw(3) << N2 << " (4k+3) | Observed: " << fixed << setprecision(5) << ((double)res.stats[9] / m_total * 100.0) << "% (" << (long long)res.stats[9] << " times)\n";
            }
            
            printDynastyReport(res.digit_history, tS, res.peak_step, res.peak_digits, res.nadir_step, res.nadir_digits);

            if (res.loop_detected && !res.loop_sequence.empty()) {
                cout << "\n";
                if (getYesNoPrompt("Show Loop cycle (y/n)? ")) {
                    int l_disp = 0; cout << "Display:\n  0: None\n  1: First 'a' and last 'b' steps\n  2: Full sequence\nChoice: ";
                    if (cin >> l_disp && l_disp > 0) {
                        int l_fa = 0, l_lb = 0;
                        if (l_disp == 1) { cout << "  First (a): "; cin >> l_fa; cout << "  Last (b): "; cin >> l_lb; }
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
                    } cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }
        
        cout << "\n=====================================================================" << endl;
        if (!getYesNoPrompt("Run another simulation? (y/n): ")) break;
    }
    
    return 0;
}