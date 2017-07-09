<?xml version="1.0" encoding="UTF-8"?>
<!-- edited with XMLSpy v2008 sp1 (http://www.altova.com) by Dave (Cab) -->
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format" xmlns:xs="http://www.w3.org/2001/XMLSchema" xmlns:fn="http://www.w3.org/2005/xpath-functions">
    <xsl:output method="text" encoding="UTF-8"/>
    <xsl:template match="/declaration">
        <xsl:variable name="vendor">
            <xsl:value-of select="system-property('xsl:vendor')"/>
        </xsl:variable>
        <xsl:if test="@name">
            <xsl:text>/// @file </xsl:text>
            <xsl:value-of select="@name"/>
            <xsl:text>_DeclarationParser.h&#10;</xsl:text>
        </xsl:if>
        <xsl:text>/// @note Vendor: </xsl:text>
        <xsl:value-of select="$vendor"/>
        <xsl:text>&#10;</xsl:text>
        <xsl:text>/// @date </xsl:text>
        <!--<xsl:value-of select="current-date()"/>-->
        <xsl:text>&#10;//&#10;&#10;</xsl:text>
        <xsl:if test="@name">
            <xsl:text>#ifndef </xsl:text>
            <xsl:value-of select="@name"/>
            <xsl:text>_DeclarationParser_h__&#10;#define </xsl:text>
            <xsl:value-of select="@name"/>
            <xsl:text>_DeclarationParser_h__&#10;&#10;</xsl:text>
        </xsl:if>
        <xsl:if test="@name">
            <xsl:text>#include "</xsl:text>
            <xsl:value-of select="@name"/>
            <xsl:text>_Declaration.h"&#10;&#10;</xsl:text>
        </xsl:if>
#ifndef member_size
#define member_size(type, member) sizeof(((type *)0)->member)
#endif
        <xsl:if test="@namespace">
/**
* @brief Declaraciones de funciones y plantillas para facilitar el acceso a las estructuras y tipos enumerados usados en la comunicación UDP del SAM.
*/
            <xsl:text>namespace SAM_DeclarationParser </xsl:text>
            <xsl:text>
{
    using namespace </xsl:text>
            <xsl:value-of select="@namespace"/>
            <xsl:text>;
</xsl:text>
        </xsl:if>
        <xsl:apply-templates select="enum[@chars &gt; 0]"/>
        <xsl:apply-templates select="struct|union|class"/>
        <xsl:text>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enums a strings: 

/** Declarado en otro sitio
struct EnumElemToString 
{
	long lElem;
	const char * szName;
};
*/

</xsl:text>
        <xsl:for-each select="enum">
            <xsl:call-template name="enumToStrings">
                <xsl:with-param name="enum" select="."/>
            </xsl:call-template>
        </xsl:for-each>
        /// Nombres de tipos de datos
        <xsl:for-each select="struct|union|class|enum">
            <xsl:if test="@name">
                /// Obtiene el nombre del tipo de datos para <xsl:value-of select="@name"/>
                template&lt;&gt;
                inline const char * getTypeName&lt;<xsl:value-of select="@name"/>&gt;()
                {
                return "<xsl:value-of select="@name"/>";
                }
            </xsl:if>
        </xsl:for-each>
        <xsl:if test="@namespace">
            <xsl:text>&#10;} ///&lt; namespace</xsl:text>
            <xsl:value-of select="@namespace"/>
            <xsl:text>&#10;</xsl:text>
        </xsl:if>
        <xsl:if test="@name">
            <xsl:text>&#10;&#10;#endif //</xsl:text>
            <xsl:value-of select="@name"/>
            <xsl:text>_DeclarationParser_h__&#10;&#10;</xsl:text>
        </xsl:if>
    </xsl:template>
    <xsl:template match="struct|union|class|enum">
        <xsl:choose>
            <xsl:when test="@name">
                <xsl:text>/// Intermediario para serializar </xsl:text>
                <xsl:value-of select="@name"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="local-name()"/>
                <xsl:text>&#10;template&lt;typename Archive&gt;
void serialize(Archive &amp; ar, </xsl:text>
                <xsl:value-of select="@name"/>
                <xsl:text> &amp; t</xsl:text>
                <xsl:text>)
{&#10;</xsl:text>
                <xsl:if test="parser">
                    <xsl:text>    </xsl:text>
                    <xsl:value-of select="parser"/>
                    <xsl:text>&#10;&#10;</xsl:text>
                </xsl:if>
                <xsl:if test=".//item[dimension/@limit]">
                    // Límite del array
                    <xsl:for-each select="(.//dimension/@limit)[1]">
                        <xsl:value-of select="ancestor::*/item[@type and @name = current()]/@type"/>
                        <xsl:text> </xsl:text><xsl:value-of select="."/> = t.<xsl:value-of select="."/>;
                    </xsl:for-each>
                </xsl:if>
                &#10;<xsl:choose>
                    <xsl:when test="local-name() = 'enum' and @chars > 0">
                        typedef char (*toarray)[<xsl:value-of select="@chars"/>];
                        ar &amp; *(toarray) &amp;t;
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:apply-templates select="item | struct[not(@name)]|union[not(@name)]|class[not(@name)]"/>
                    </xsl:otherwise>
                </xsl:choose>
                <xsl:text>}&#10;&#10;</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:apply-templates select="item | struct[not(@name)]|union[not(@name)]|class[not(@name)]"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <xsl:template match="item">
        <xsl:if test="../../union">
            <xsl:choose>
                <xsl:when test="when/@source and when/text()">
                    <xsl:variable name="source">
                        <xsl:value-of select="when/@source"/>
                    </xsl:variable>
                    <xsl:if test="not(position()=1)">
                        <xsl:text> else</xsl:text>
                    </xsl:if>
                    <xsl:text> if( </xsl:text>
                    <xsl:if test="not(ancestor::*/argument[text() = $source])">
                        <xsl:text>t.</xsl:text>
                    </xsl:if>
                    <xsl:value-of select="when/@source"/>
                    <xsl:text> == </xsl:text>
                    <xsl:value-of select="when/text()"/>
                    <xsl:text> )
    </xsl:text>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:text> else
    </xsl:text>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:if>
        <xsl:text>     { </xsl:text>
        <xsl:choose>
            <xsl:when test="dimension/@limit">
                <xsl:text>ar &amp; AR_SERIALIZATION_L(t, </xsl:text>
                <xsl:value-of select="@name"/>, <xsl:value-of select="dimension/@limit"/>);
            </xsl:when>

            <xsl:otherwise>
                <xsl:text>ar &amp; AR_SERIALIZATION_M(t, </xsl:text>
                <xsl:value-of select="@name"/>
                <xsl:text>); </xsl:text>
                <xsl:if test="@func">
                    t.<xsl:value-of select="@name"/> = (<xsl:value-of select="@type"/>)<xsl:value-of select="@func"/><xsl:text>(</xsl:text>(unsigned long) t.<xsl:value-of select="@name"/><xsl:text>);</xsl:text>
                </xsl:if>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:if test="../../union/@greater and (../../union/@greater != @name)">
            uint8 _dummy____[member_size(<xsl:value-of select="../../@name"/>, <xsl:value-of select="../../union/@greater"/>)-member_size(<xsl:value-of select="../../@name"/>, <xsl:value-of select="@name"/>)];
            ar &amp; AR_SERIALIZATION(_dummy____);
        </xsl:if>
        <xsl:text> }</xsl:text>
        <xsl:apply-templates select="comment"/>
        <xsl:if test="@source">
            <xsl:text> ///&lt; [Toma los valores desde </xsl:text>
            <xsl:value-of select="@source"/>
            <xsl:text>] </xsl:text>
        </xsl:if>
        <xsl:text>&#10;</xsl:text>
    </xsl:template>
    <xsl:template match="comment">
        <xsl:if test="preceding-sibling::comment">
            <xsl:text>&#10;        </xsl:text>
        </xsl:if>
        <xsl:text> ///&lt; </xsl:text>
        <xsl:value-of select="normalize-space(text())"/>
    </xsl:template>
    <xsl:template name="enumToStrings">
        <xsl:param name="enum"/>
        <xsl:text>static EnumElemToString&lt;</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>> g_aEnumElemToString_</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>[] =
{
</xsl:text>
        <xsl:for-each select="$enum/item[@name]">
            <xsl:text>    { </xsl:text>
            <xsl:value-of select="@name"/>
            <xsl:text>, "</xsl:text>
            <xsl:value-of select="@name"/>
            <xsl:text>" },
</xsl:text>
        </xsl:for-each>
        <xsl:text>    { (</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>)  ~0, NULL }
};

</xsl:text>
        <xsl:text>inline bool getEnumElemId( </xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text> &amp; elem, const char * szElemName)
{
        for(const EnumElemToString&lt;</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>> * it = </xsl:text>
        <xsl:text> g_aEnumElemToString_</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>; it &amp;&amp; it->szName; ++it)
        {
            if(!strcmp(szElemName, it->szName))
            {
                elem = it->lElem;
                return true;
            }
        }

        return false;
}

</xsl:text>
        <xsl:text>inline bool getEnumElemName( const char ** pszElemName, const </xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text> &amp; elem )
{
        for(const EnumElemToString&lt;</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>> * it = </xsl:text>
        <xsl:text> g_aEnumElemToString_</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>; it &amp;&amp; it->szName; ++it)
        {
            if(elem == it->lElem)
            {
                if(pszElemName)
                    *pszElemName = it->szName;
                return true;
            }
        }

        return false;
}

</xsl:text>
        <xsl:text>inline size_t getEnumElem( EnumElemToString&lt;  </xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text> > * pElem, size_t idx )
{
    const size_t nElems = sizeof(g_aEnumElemToString_</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>) / sizeof(*</xsl:text>
        <xsl:text> g_aEnumElemToString_</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>);
  </xsl:text>
        <xsl:text>
    if(idx &lt; nElems &amp;&amp; pElem)
    {
        *pElem = g_aEnumElemToString_</xsl:text>
        <xsl:value-of select="$enum/@name"/>
        <xsl:text>[idx];
    }

    return nElems;
}

</xsl:text>
    </xsl:template>
</xsl:stylesheet>
