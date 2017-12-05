#include <algorithm>    // std::reverse

class Solution 
{
public:
    string reverseString(string s) 
    {
        string result = s;
        reverse(result.begin(), result.end());
        //result.assign(s.rbegin(), s.rend());
        return result;
    }
};