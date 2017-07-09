<?xml version="1.0" encoding="UTF-8"?>
<!-- edited with XMLSpy v2008 sp1 (http://www.altova.com) by Dave (Cab) -->
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text" encoding="UTF-8"/>
  <xsl:template match="/declaration">
    <xsl:variable name="vendor">
      <xsl:value-of select="system-property('xsl:vendor')"/>
    </xsl:variable>
    <xsl:if test="@name">
      <xsl:text>/// @file </xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>_Declaration.h&#10;</xsl:text>
    </xsl:if>
    <xsl:text>/// @note Vendor: </xsl:text>
    <xsl:value-of select="$vendor"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:text>&#10;//&#10;&#10;</xsl:text>
    <xsl:if test="@name">
      <xsl:text>#ifndef </xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>_Declaration_h__&#10;#define </xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>_Declaration_h__&#10;&#10;</xsl:text>
    </xsl:if>
    <xsl:text>#include "SAM_DeclarationCommon.h"

</xsl:text>
    <xsl:text>
/// Encode as big endian. That is a network independent code.
#define CHARTOLONG_BE(x4, x3, x2, x1) ((uint32)(uint8)(x1) | ((uint32)(uint8)(x2) &lt;&lt; 8) | ((uint32)(uint8)(x3) &lt;&lt; 16) | ((uint32)(uint8)(x4) &lt;&lt; 24))
#define CHARTOLONG_LE(x4, x3, x2, x1) ((uint32)(uint8)(x4) | ((uint32)(uint8)(x3) &lt;&lt; 8) | ((uint32)(uint8)(x2) &lt;&lt; 16) | ((uint32)(uint8)(x1) &lt;&lt; 24))

#ifndef CHARTOLONG
#if defined(BIG_ENDIAN_OR_PPC) 
#define CHARTOLONG CHARTOLONG_BE
#else
#define CHARTOLONG CHARTOLONG_LE
#endif
#endif
</xsl:text>
    <xsl:apply-templates select="table"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:apply-templates select="define"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:text>namespace SAM {} // Reserva el namespace SAM por si no lo estuviera.

</xsl:text>
    <xsl:if test="@namespace">
      /**
      * @brief Declaraciones de estructuras y tipos enumerados usados en la comunicación UDP a bajo
      *     nivel con otros módulos del SCV.
      */
      <xsl:text>namespace </xsl:text>
      <xsl:value-of select="@namespace"/>
      <xsl:text>&#10;{&#10;#ifdef _WIN32
#pragma pack(push, 1)
#endif
&#10;</xsl:text>
    </xsl:if>
    <xsl:text>    using namespace SAM;
    using namespace SAM_DeclarationParser;
</xsl:text>
    <xsl:apply-templates select="typedef"/>
    <xsl:text>&#10;</xsl:text>
    <xsl:apply-templates select="enum"/>
    <xsl:text>&#10;&#10;</xsl:text>
    <xsl:apply-templates select="struct|union|class"/>
    <xsl:if test="@namespace">
      <xsl:text>&#10;#ifdef _WIN32
#pragma pack(pop)
#endif
&#10;} // namespace </xsl:text>
      <xsl:value-of select="@namespace"/>
      <xsl:text>&#10;</xsl:text>
    </xsl:if>
    <xsl:if test="@name">
      <xsl:text>&#10;&#10;#endif //</xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:text>_Declaration_h__&#10;&#10;</xsl:text>
    </xsl:if>
  </xsl:template>
  <xsl:template match="table">
    <xsl:text>/// @note Usando la tabla </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>: </xsl:text>
    <xsl:for-each select="item">
      <xsl:value-of select="text()"/>
      <xsl:text> </xsl:text>
    </xsl:for-each>
    <xsl:apply-templates select="comment"/>
    <xsl:text>&#10;</xsl:text>
  </xsl:template>
  <xsl:template match="typedef">
    <xsl:text>typedef </xsl:text>
    <xsl:value-of select="@aliased"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>;</xsl:text>
    <xsl:apply-templates select="comment"/>
    <xsl:text>&#10;</xsl:text>
  </xsl:template>
  <xsl:template match="define">
    <xsl:text>#define </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:if test="@arguments">
      <xsl:text>(</xsl:text>
      <xsl:value-of select="@arguments"/>
      <xsl:text>)</xsl:text>
    </xsl:if>
    <xsl:text> </xsl:text>
    <xsl:value-of select="text()"/>
    <xsl:apply-templates select="comment"/>
    <xsl:text>&#10;</xsl:text>
  </xsl:template>
  <xsl:template match="struct|union|class">
    <xsl:value-of select="name()"/>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:if test="extends">
      <xsl:text> : </xsl:text>
      <xsl:for-each select="extends">
        <xsl:value-of select="@access"/>
        <xsl:text> </xsl:text>
        <xsl:value-of select="@type"/>
        <xsl:if test="not(position()=last())">
          <xsl:text>,</xsl:text>
        </xsl:if>
      </xsl:for-each>
    </xsl:if>
    <xsl:text/>
    <xsl:apply-templates select="comment"/>
    <xsl:text>&#10;{&#10;</xsl:text>
    <xsl:apply-templates select="item[../../struct|../../union|../../class]|struct|union|class"/>
    <xsl:text>} TIGHLY_PACKED;&#10;&#10;</xsl:text>
  </xsl:template>
  <xsl:template match="item[(ancestor::struct|ancestor::union|ancestor::class)]">
    <xsl:text>    </xsl:text>
      <xsl:if test="@source">Enum&lt;<xsl:value-of select="@source"/>, <xsl:value-of select="@type"/>&gt;</xsl:if>
      <xsl:if test="not(@source)">
          <xsl:value-of select="@type"/>
      </xsl:if>
      <xsl:text> </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:if test="dimension">
      <xsl:text>[</xsl:text>
        <xsl:value-of select="normalize-space(dimension/text())"/>
        <xsl:apply-templates select="dimension/macro"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
    <xsl:text>;</xsl:text>
    <xsl:if test="dimension/@limit">
      <xsl:text> ///&lt; [Limitado por el miembro ´</xsl:text>
      <xsl:value-of select="dimension/@limit"/>
      <xsl:text>´]</xsl:text>
    </xsl:if>
    <xsl:apply-templates select="comment"/>
    <xsl:if test="@table">
      <xsl:text> ///&lt; [Toma los valores de la tabla </xsl:text>
      <xsl:value-of select="@source"/>
      <xsl:text>] </xsl:text>
    </xsl:if>
    <xsl:if test="when">
      <xsl:text> ///&lt; [Usado cuando el miembro </xsl:text>
      <xsl:value-of select="when/@source"/>
      <xsl:text> es </xsl:text>
      <xsl:value-of select="when/text()"/>
      <xsl:text>]</xsl:text>
    </xsl:if>
    <xsl:text>&#10;</xsl:text>
  </xsl:template>
  <xsl:template match="enum">
    <xsl:text>enum </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:apply-templates select="comment"/>
    <xsl:text>&#10;{&#10;</xsl:text>
    <xsl:for-each select="item">
      <xsl:text>    </xsl:text>
      <xsl:value-of select="@name"/>
      <xsl:if test="text()|macro">
        <xsl:variable name="value">
        <xsl:for-each select="node()">
          <xsl:if test="not(name())">
           <xsl:value-of select="normalize-space()"/>
          </xsl:if>
        </xsl:for-each>
        </xsl:variable>
        <xsl:if test="$value!='' or macro">
          <xsl:text> = </xsl:text>
          <xsl:value-of select="$value"/>
        </xsl:if>
        <xsl:apply-templates select="macro"/>
      </xsl:if>
      <xsl:if test="position() != last()"><xsl:text>,</xsl:text></xsl:if>
      <xsl:apply-templates select="comment"/>
      <xsl:text>&#10;</xsl:text>
    
    </xsl:for-each>
    <xsl:text>};&#10;&#10;</xsl:text>
  </xsl:template>
  <xsl:template match="macro[@ref]">
    <xsl:value-of select="@ref"/>
    <xsl:if test="text()">
      <xsl:text>(</xsl:text>
      <xsl:value-of select="text()"/>
      <xsl:text>)</xsl:text>
    </xsl:if>
  </xsl:template>
  <xsl:template match="comment">
    <xsl:if test="preceding-sibling::comment">
      <xsl:text>&#10;       /// </xsl:text>
    </xsl:if>
    <xsl:if test="not(preceding-sibling::comment)">
      <xsl:text> ///</xsl:text>
      <xsl:if test="not(../struct|../union|../class|../enum|../../struct|../../union|../../class|../../enum)">
        <xsl:text>&lt;</xsl:text>
      </xsl:if>
      <xsl:text> </xsl:text>
    </xsl:if>
    <xsl:value-of select="normalize-space(text())"/>
  </xsl:template>
</xsl:stylesheet>
