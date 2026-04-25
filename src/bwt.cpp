#include "bwt.hpp"

#include <algorithm>
#include <array>
#include <stdexcept>
#include <vector>

using namespace std;

namespace bwt {

pair<vector<uint8_t>, uint32_t> transform(
    const vector<uint8_t>& input
) {
    const int n = static_cast<int>(input.size());
    if (n == 0) {
        return {{}, 0};
    }

    vector<int> p(n), c(n);
    array<int, 256> cnt{};
    for (uint8_t ch : input) {
        ++cnt[ch];
    }
    for (int i = 1; i < 256; ++i) {
        cnt[i] += cnt[i - 1];
    }
    for (int i = n - 1; i >= 0; --i) {
        p[--cnt[input[i]]] = i;
    }

    c[p[0]] = 0;
    int classes = 1;
    for (int i = 1; i < n; ++i) {
        if (input[p[i]] != input[p[i - 1]]) {
            ++classes;
        }
        c[p[i]] = classes - 1;
    }

    vector<int> pn(n), cn(n), class_count;
    for (int h = 0; (1 << h) < n; ++h) {
        for (int i = 0; i < n; ++i) {
            pn[i] = p[i] - (1 << h);
            if (pn[i] < 0) {
                pn[i] += n;
            }
        }

        class_count.assign(classes, 0);
        for (int i = 0; i < n; ++i) {
            ++class_count[c[pn[i]]];
        }
        for (int i = 1; i < classes; ++i) {
            class_count[i] += class_count[i - 1];
        }
        for (int i = n - 1; i >= 0; --i) {
            p[--class_count[c[pn[i]]]] = pn[i];
        }

        cn[p[0]] = 0;
        classes = 1;
        for (int i = 1; i < n; ++i) {
            const pair<int, int> cur = {c[p[i]], c[(p[i] + (1 << h)) % n]};
            const pair<int, int> prev = {c[p[i - 1]], c[(p[i - 1] + (1 << h)) % n]};
            if (cur != prev) {
                ++classes;
            }
            cn[p[i]] = classes - 1;
        }
        c.swap(cn);
    }

    vector<uint8_t> last_col(n);
    uint32_t primary_index = 0;
    for (int i = 0; i < n; ++i) {
        const int start = p[i];
        last_col[i] = input[(start + n - 1) % n];
        if (start == 0) {
            primary_index = static_cast<uint32_t>(i);
        }
    }

    return {last_col, primary_index};
}

vector<uint8_t> inverse_transform(
    const vector<uint8_t>& input, uint32_t primary_index
) {
    const size_t n = input.size();
    if (n == 0) {
        return {};
    }
    if (primary_index >= n) {
        throw runtime_error("Invalid BWT primary index.");
    }

    array<uint32_t, 256> count{};
    vector<uint32_t> occ_rank(n);
    for (size_t i = 0; i < n; ++i) {
        const uint8_t ch = input[i];
        occ_rank[i] = count[ch];
        ++count[ch];
    }

    array<uint32_t, 256> starts{};
    uint32_t sum = 0;
    for (size_t c = 0; c < 256; ++c) {
        starts[c] = sum;
        sum += count[c];
    }

    vector<uint32_t> next(n);
    for (size_t i = 0; i < n; ++i) {
        next[i] = starts[input[i]] + occ_rank[i];
    }

    vector<uint8_t> output(n);
    uint32_t idx = primary_index;
    for (size_t k = n; k > 0; --k) {
        output[k - 1] = input[idx];
        idx = next[idx];
    }

    return output;
}

}  // namespace bwt
