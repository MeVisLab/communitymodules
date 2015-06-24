<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version='1.0'>

  <!-- import global MeVis stylesheet -->
  <xsl:import href="$${MLAB_MeVis_BuildSystem_URL}/BuildTools/DocTools/Files/xsl/docbook/mevis/fo.xsl"/>

  <!-- remove tabel of figures for book -->
    <xsl:param name="generate.toc">
      appendix  toc,title
      article/appendix  nop
      article   toc,title
      book      toc,title
      chapter   title
      part      title
      preface   title
      qandadiv  toc
      qandaset  toc
      reference title
      sect1     toc
      sect2     toc
      sect3     toc
      sect4     toc
      sect5     toc
      section   toc
      set       title
    </xsl:param>

 
  <!-- enable double sided layouting for the printed book -->
  <xsl:param name="double.sided">0</xsl:param>

  <!-- Add your own customization here. To see what can be customized, 
       refer to the global MeVis stylesheet above 
    -->
  <xsl:param name="title.watermark.image" select="''"/>
</xsl:stylesheet>

