#include "Attributes.h"
#include <algorithm>

namespace xUnitAttributes
{

xUnitpp::AttributeCollection Attributes()
{
    return xUnitpp::AttributeCollection();
}

}

namespace xUnitpp
{

AttributeCollection::AttributeCollection()
    : skipped(std::make_pair(false, ""))
{
}

void swap(AttributeCollection &a, AttributeCollection &b)
{
    using std::swap;
    swap(a.sortedAttributes, b.sortedAttributes);
    swap(a.skipped, b.skipped);
}

void AttributeCollection::insert(Attribute &&a)
{
    sortedAttributes.push_back(a);

    if (a.first == "Skip")
    {
        skipped.first = true;
        skipped.second = a.second;
    }
}

const std::pair<bool, std::string> &AttributeCollection::Skipped() const
{
    return skipped;
}

bool AttributeCollection::empty() const
{
    return sortedAttributes.empty();
}

AttributeCollection::const_iterator AttributeCollection::begin() const
{
    return sortedAttributes.begin();
}

AttributeCollection::const_iterator AttributeCollection::end() const
{
    return sortedAttributes.end();
}

void AttributeCollection::sort()
{
    std::sort(sortedAttributes.begin(), sortedAttributes.end());
}

AttributeCollection::iterator_range AttributeCollection::find(const Attribute &att) const
{
    return std::equal_range(begin(), end(), att, [](const Attribute &a0, const Attribute &a1) { return a0.first < a1.first; });
}

}
