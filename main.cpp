#include <bits/stdc++.h>
using namespace std;

static inline string trim(const string &x)
{
    size_t a = 0, b = x.size();
    while (a < b && isspace(static_cast<unsigned char>(x[a])))
    {
        ++a;
    }
    while (b > a && isspace(static_cast<unsigned char>(x[b - 1])))
    {
        --b;
    }
    return x.substr(a, b - a);
}

static inline vector<string> split(const string &s, char delim)
{
    vector<string> out;
    string cur;
    for (char c : s)
    {
        if (c == delim)
        {
            out.push_back(cur);
            cur.clear();
        }
        else
            cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}

static inline bool has_upper(const string &t)
{
    for (char c : t)
    {
        if ('A' <= c && c <= 'Z')
        {
            return true;
        }
    }
    return false;
}

size_t minlen_of(const string &t,
                 const vector<char> &vars,
                 const vector<vector<string>> &domains)
{
    size_t len = 0;
    for (char c : t)
    {
        if ('a' <= c && c <= 'z')
        {
            len += 1;
        }
        else if ('A' <= c && c <= 'Z')
        {
            auto it = find(vars.begin(), vars.end(), c);
            if (it == vars.end())
            {
                return SIZE_MAX;
            }
            size_t vi = size_t(it - vars.begin());

            size_t best = SIZE_MAX;
            for (const auto &r : domains[vi])
            {
                best = min(best, r.size());
            }

            if (best == SIZE_MAX)
                return SIZE_MAX;
            len += best;
        }
    }
    return len;
}

static inline string expand_pattern(const string &pat, const unordered_map<char, string> &assign)
{
    string out;
    out.reserve(pat.size() * 2);
    for (char c : pat)
    {
        if ('A' <= c && c <= 'Z')
        {
            auto it = assign.find(c);
            if (it == assign.end())
                return string();
            out += it->second;
        }
        else
        {
            out.push_back(c);
        }
    }
    return out;
}

static inline bool is_substring(const string &sub, const string &str)
{
    if (str.size() > sub.size())
    {
        return false;
    }
    return sub.find(str) != string::npos;
}

bool dfs(int depth,
         const vector<char> &vars,
         const vector<int> &inv_order,
         const vector<vector<string>> &domains,
         const vector<string> &patterns,
         const string &s,
         unordered_map<char, string> &assign,
         vector<pair<char, string>> &solution)
{
    int m = (int)vars.size();
    if (depth == m)
    {
        for (const string &pat : patterns)
        {
            string expd;
            expd.reserve(pat.size());
            for (char c : pat)
            {
                if ('A' <= c && c <= 'Z')
                    expd += assign[c];
                else
                    expd.push_back(c);
            }
            if (s.find(expd) == string::npos)
                return false;
        }
        solution.clear();
        for (char v : vars)
            solution.emplace_back(v, assign[v]);
        return true;
    }

    int vi = inv_order[depth];
    char vch = vars[vi];

    for (const string &val : domains[vi])
    {
        assign[vch] = val;

        bool ok = true;
        for (const string &pat : patterns)
        {
            bool needs = false;
            for (char c : pat)
            {
                if ('A' <= c && c <= 'Z' && !assign.count(c))
                {
                    needs = true;
                    break;
                }
            }
            if (!needs)
            {
                string expd;
                for (char c : pat)
                    expd += ('A' <= c && c <= 'Z') ? assign[c] : string(1, c);
                if (s.find(expd) == string::npos)
                {
                    ok = false;
                    break;
                }
            }
        }

        if (ok && dfs(depth + 1, vars, inv_order, domains, patterns, s, assign, solution))
            return true;

        assign.erase(vch);
    }
    return false;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<string> raw;
    for (string line; getline(cin, line);)
    {
        string t = trim(line);
        if (!t.empty())
            raw.push_back(t);
    }
    if (raw.empty())
    {
        cout << "NO\n";
        return 0;
    }

    size_t idx = 0;

    // Nuber of patterns
    long long k = -1;
    try
    {
        k = stoll(raw[idx++]);
    }
    catch (...)
    {
        cout << "NO\n";
        return 0;
    }
    if (k < 0)
    {
        cout << "NO\n";
        return 0;
    }

    // Base string
    if (idx >= raw.size())
    {
        cout << "NO\n";
        return 0;
    }
    string s = raw[idx++];

    // Patterns
    if (idx + static_cast<size_t>(k) > raw.size())
    {
        cout << "NO\n";
        return 0;
    }
    vector<string> patterns;
    patterns.reserve((size_t)k);
    for (long long i = 0; i < k; ++i)
    {
        patterns.push_back(raw[idx++]);
    }

    // Expansions
    unordered_map<char, vector<string>> R;
    for (; idx < raw.size(); ++idx)
    {
        const string &ln = raw[idx];
        size_t pos = ln.find(':');
        if (pos == string::npos || pos == 0)
        {
            cout << "NO\n";
            return 0;
        }
        if (pos != 1)
        {
            cout << "NO\n";
            return 0;
        }
        char var = ln[0];
        if (!(var >= 'A' && var <= 'Z'))
        {
            cout << "NO\n";
            return 0;
        }
        string rhs = ln.substr(2);
        vector<string> items = split(rhs, ',');
        vector<string> vals;
        vals.reserve(items.size());
        for (auto &w : items)
        {
            string v = trim(w);
            bool ok = true;
            for (char c : v)
            {
                if (!(c >= 'a' && c <= 'z'))
                {
                    ok = false;
                    break;
                }
            }
            if (!ok)
            {
                cout << "NO\n";
                return 0;
            }
            vals.push_back(v);
        }
        R[var] = move(vals);
    }

    set<char> vars_set;
    for (const string &t : patterns)
    {
        for (char c : t)
        {
            if ('A' <= c && c <= 'Z')
            {
                vars_set.insert(c);
            }
        }
    }

    for (char v : vars_set)
    {
        if (!R.count(v))
        {
            cout << "NO\n";
            return 0;
        }
    }

    for (const string &t : patterns)
    {
        if (!has_upper(t))
        {
            for (char c : t)
            {
                if (!(c >= 'a' && c <= 'z'))
                {
                    cout << "NO\n";
                    return 0;
                }
            }
            if (!is_substring(s, t))
            {
                cout << "NO\n";
                return 0;
            }
        }
    }

    vector<char> vars(vars_set.begin(), vars_set.end());
    size_t m = vars.size();
    vector<vector<string>> domains(m);
    for (size_t i = 0; i < m; ++i)
    {
        char v = vars[i];
        auto &src = R[v];
        // only keep values that occur in s and |r| <= |s|
        for (const string &r : src)
        {
            if (r.size() <= s.size() && is_substring(s, r))
            {
                domains[i].push_back(r);
            }
        }
        if (domains[i].empty())
        {
            cout << "NO\n";
            return 0;
        }
    }
    for (const auto &t : patterns)
    {
        size_t ml = minlen_of(t, vars, domains);
        if (ml == SIZE_MAX || ml > s.size())
        {
            cout << "NO\n";
            return 0;
        }
    }
    vector<int> inv_order(m);
    iota(inv_order.begin(), inv_order.end(), 0);
    unordered_map<char, string> assign;
    assign.reserve(m);
    vector<pair<char, string>> solution;

    bool found = dfs(0, vars, inv_order, domains, patterns, s, assign, solution);
    if (!found)
    {
        cout << "NO\n";
        return 0;
    }

    sort(solution.begin(), solution.end(), [](const auto &a, const auto &b)
         { return a.first < b.first; });
    for (auto &p : solution)
    {
        cout << p.first << ":" << p.second << "\n";
    }
    return 0;
}