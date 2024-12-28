//#include "ISymbol.h"
//
//using namespace std;
//using namespace OmniParser;
//
//vector<SymbolTypeDescriptor> ISymbol::m_typeDescriptors = vector<SymbolTypeDescriptor>();
//
//int ISymbol::RegisterType(SymbolTypeDescriptor descriptor)
//{
//    m_typeDescriptors.push_back(descriptor);
//    return m_typeDescriptors.size() - 1;
//}
//
//bool OmniParser::SymbolTypeDescriptor::operator==(const SymbolTypeDescriptor& self, const SymbolTypeDescriptor& other)
//{
//    return self.typeId == other.typeId;
//}
