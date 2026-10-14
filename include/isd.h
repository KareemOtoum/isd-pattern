#pragma once

#include <meta>
#include <vector>

namespace isd
{

consteval bool same_signature(std::meta::info A, std::meta::info B) {
    using namespace std::meta;

    if(!has_identifier(A) || !has_identifier(B)) {
        throw "A function involved in an interface "
            "check does not have an identifier";
    }

    auto aname = identifier_of(A);
    auto bname = identifier_of(B);

    return aname == bname && is_same_type(type_of(A), type_of(B));
}

// can pass as params, dont need templates
consteval bool implements(std::meta::info objInfo, std::meta::info interfaceInfo) {
    using namespace std::meta;

    auto ctx = access_context::current();

    auto interfaceMembers = members_of(interfaceInfo, ctx);
    auto objMembers = members_of(objInfo, ctx);

    std::vector<info> objFunctions;
    for(auto m : objMembers) {
        // only consider identifiable functions for impl
        if(is_function(m) && has_identifier(m)) {
            objFunctions.push_back(m);
        }
    }

    bool implements{ true };
    for(auto m : interfaceMembers) {
        // only match require functions for now
        if(!is_function(m) || !is_pure_virtual(m)) continue;

        // obj must have function with same signature
        bool found{ false };

        for(auto funcInfo : objFunctions) {
            // must be implemented only once
            if(same_signature(funcInfo, m)) {
                if(found) {
                    implements = false;
                } else {
                    found = true;
                }
            }
        }

        if(!found) {
            implements = false;
        }
    }

    return implements;
}

} // namespace isd