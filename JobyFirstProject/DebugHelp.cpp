//
//  DebugHelp.cpp
//  Demo CPP Project
//
//  Created by Chad Mitchell on 1/15/25.
//

#include <iostream>
#include <map>
#include <random>

#include "DebugHelp.hpp"

/*
auto isOdd = [](int candidate) {return candidate % 2 != 0; };
// [] can capture specific variables or all
 [x,y] – capture x and y by value
- Copies are made
- Lambda can be used when x and y have gone
out of scope
[&x,&y] – capture x and y by reference
- No copies, changes affect the originals
- Dangling references may be an issue
[x=a+1,y=std::move(b)] – alias or move capture
 [x,y] – capture x and y by value
 - Copies are made
 - Lambda can be used when x and y have gone
 out of scope
 [&x,&y] – capture x and y by reference
 - No copies, changes affect the originals
 - Dangling references may be an issue
 [x=a+1,y=std::move(b)] – alias or move capture
*/
/*
 void examples(int i) {
    std::vector<int> v{1, 2,3};
    v.push_back(4);
    begin(v);end(v); // better choice if exists
    v.begin();v.end();
    int const target = 2;
    long simple = count(begin(v),end(v), target);
    
    long odds = count_if(begin(v), end(v), [](auto elem) {return elem % 2 != 0; });
    
    std::map<int, int> monthlengths{ { 1,31 },{ 2,28 },{ 3,31 },{ 4,30 },{ 5,31 },{ 6,30 },{ 7,31 },{ 8,31 },{ 9,30 },{ 10,31 },{ 11,30 },{ 12,31 } };
    
    long longmonths = count_if(begin(monthlengths), end(monthlengths), [](auto elem) {return elem.second == 31; });
    
    //are all, any, or none of the values odd? (Conclude from number of odd entries)
    bool allof = all_of(begin(v), end(v), [](auto elem) {return elem % 2 != 0; });
    bool noneof = none_of(begin(v), end(v), [](auto elem) {return elem % 2 != 0; });
    bool anyof = any_of(begin(v), end(v), [](auto elem) {return elem % 2 != 0; });
    
    std::vector<int> v1{ 4,1,0,1,-2,3,7,-6,2,0,0,-9,9 };
    auto v2 = v1;
    sort(begin(v2), end(v2));
    sort(begin(v2), end(v2), [](int elem1, int elem2)  {return elem1 > elem2; });
    
    std::random_device randomdevice;
    std::mt19937 generator(randomdevice());
    shuffle(begin(v2), end(v2), generator);
    
    std::vector<int> a{ 1, 2, 3, 4 , 5};
    std::vector<int> b{ 1, 2, 0, 4 };
    bool same = equal(begin(a), end(a), begin(b), end(b));
    auto firstchange = mismatch(begin(a), end(a), begin(b));
    
    int total = accumulate(begin(a), end(a), 0);
    total = accumulate(begin(a), end(a), 0,
                       [](int total, int i) {if (i % 2 == 0) return total + i; return total; });
    total = accumulate(begin(a), end(a), 1,
                       [](int total, int i) {return total * i; });
    
    for_each(begin(b), end(b), [](int& elem) {elem = 2; });
    b = a;
    auto firstthree = find(begin(b), end(b), 3);
    for_each(firstthree, end(b), [](int& elem) {elem = 0; });
    
    std::pair<int, std::string>aPair(42,"test");
    std::cout << aPair.second << std::endl;
    std::tuple<int, std::string, double>aTuple(42,"test",3.14);
    const auto [anInt, aString, aDouble] = aTuple;
    
    std::shared_ptr<int> testPtr = make_shared<int>(a);
    std::cout << "use count: " << testPtr.use_count() << std::endl;
    {
        std::shared_ptr<int> testPtr2{testPtr};
        std::cout << "use count2: " << testPtr.use_count() << std::endl;
    }
    std::cout << "use count3: " << testPtr.use_count() << std::endl;
    testPtr.reset();
    std::cout << "use count4: " << testPtr.use_count() << std::endl;


}
*/
