#pragma once

#include <SyntaxNode.h>
#include <vector>

#include "File.h"
#include "HeaderGenerator.h"
#include "PostProcessor.h"
#include "SourceGenerator.h"
#include "Translator.h"


class Generator
{

    std::vector<File> generate(PlantUml::SyntaxNode root) = 0;

    Translator m_translator;

    PostProcessor m_postProcessor;

    HeaderGenerator m_headerGenerator;

    SourceGenerator m_sourceGenerator;

};
