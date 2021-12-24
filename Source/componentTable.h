/*
  ==============================================================================

    componentTable.h
    Created: 21 Dec 2021 3:38:26pm
    Author:  Garrett Eckl

  ==============================================================================
*/

#pragma once

class componentTable    : public juce::Component,
                          public juce::TableListBoxModel
{
public:
    componentTable(juce::Array < juce::Array <juce::String>> *c)
    {

        addAndMakeVisible (table);

        table.setColour (juce::ListBox::outlineColourId, juce::Colours::grey);
        table.setOutlineThickness (1);
        
        table.getHeader().addColumn("Component", 0, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        table.getHeader().addColumn("Name", 1, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        table.getHeader().addColumn("Type", 2, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        table.getHeader().addColumn("Min", 3, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        table.getHeader().addColumn("Max", 4, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        table.getHeader().addColumn("Init", 5, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        table.getHeader().addColumn("Interval", 6, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        table.getHeader().addColumn("Size", 7, 100, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);
        table.getHeader().addColumn("Select", 8, 50, 30, -1, juce::TableHeaderComponent::ColumnPropertyFlags::notSortable);

        table.getHeader().setSortColumnId (1, true);

        table.setMultipleSelectionEnabled (true);
        componentsArray=c;
        
        numRows=componentsArray->size();
    }

    int getNumRows() override
    {
        return numRows;
    }

    void paintRowBackground (juce::Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        auto alternateColour = getLookAndFeel().findColour (juce::ListBox::backgroundColourId)
                                               .interpolatedWith (getLookAndFeel().findColour (juce::ListBox::textColourId), 0.03f);
        if (rowIsSelected)
            g.fillAll (juce::Colours::lightblue);
        else if (rowNumber % 2)
            g.fillAll (alternateColour);
    }

    void paintCell (juce::Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool rowIsSelected) override
    {
        g.setColour (rowIsSelected ? juce::Colours::darkblue : getLookAndFeel().findColour (juce::ListBox::textColourId));
        g.setFont (font);
        
        if(componentsArray->getReference(rowNumber)[columnId] != "")
        {
            auto text = componentsArray->getReference(rowNumber)[columnId];
            g.drawText (text, 2, 0, width - 4, height, juce::Justification::centredLeft, true);
        }

        g.setColour (getLookAndFeel().findColour (juce::ListBox::backgroundColourId));
        g.fillRect (width - 1, 0, 1, height);
    }
    
    void updateContent ()
    {
        DBG("Update Content!!!");
        numRows=componentsArray->size();
        table.updateContent();
        //table.repaint();
    }

    /*void sortOrderChanged (int newSortColumnId, bool isForwards) override
    {
        if (newSortColumnId != 0)
        {
            TutorialDataSorter sorter (getAttributeNameForColumnId (newSortColumnId), isForwards);
            dataList->sortChildElements (sorter);

            table.updateContent();
        }
    }*/

    Component* refreshComponentForCell (int rowNumber, int columnId, bool,
                                        Component* existingComponentToUpdate) override
    {
        if (columnId == 8)
        {
            auto* selectionBox = static_cast<SelectionColumnCustomComponent*> (existingComponentToUpdate);

            if (selectionBox == nullptr)
                selectionBox = new SelectionColumnCustomComponent (*this);

            selectionBox->setRowAndColumn (rowNumber, columnId);
            return selectionBox;
        }

        jassert (existingComponentToUpdate == nullptr);
        return nullptr;
    }

    int getColumnAutoSizeWidth (int columnId) override
    {
        if (columnId == 9)
            return 50;

        int widest = 32;

        for (auto i = getNumRows(); --i >= 0;)
        {
            if (auto* rowElement = dataList->getChildElement (i))
            {
                auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));

                widest = juce::jmax (widest, font.getStringWidth (text));
            }
        }

        return widest + 8;
    }

    juce::String getSelection (const int rowNumber) const
    {
        return componentsArray->getReference(rowNumber)[6];
        //return dataList->getChildElement (rowNumber)->getIntAttribute ("Select");
    }

    void setSelection (const int rowNumber, const bool newSelection)
    {
        std::cout << std::boolalpha << newSelection;
        if(newSelection==false)
        {
            componentsArray->getReference(rowNumber).set(6,"OFF");
            DBG(componentsArray->getReference(rowNumber)[6]);
        }
        else if(newSelection==true)
        {
            componentsArray->getReference(rowNumber).set(6,"ON");
            DBG(componentsArray->getReference(rowNumber)[6]);
        }
        //dataList->getChildElement (rowNumber)->setAttribute ("Select", newSelection);
    }

    juce::String getText (const int columnNumber, const int rowNumber) const
    {
        return dataList->getChildElement (rowNumber)->getStringAttribute (getAttributeNameForColumnId (columnNumber));
    }

    void setText (const int columnNumber, const int rowNumber, const juce::String& newText)
    {
        const auto& columnName = table.getHeader().getColumnName (columnNumber);
        dataList->getChildElement (rowNumber)->setAttribute (columnName, newText);
    }

    //==============================================================================
    void resized() override
    {
        table.setBoundsInset (juce::BorderSize<int> (8));
    }
    
    int numRows = 0;

private:
    juce::TableListBox table  { {}, this };
    juce::Font font           { 14.0f };

    std::unique_ptr<juce::XmlElement> tutorialData;
    juce::XmlElement* columnList = nullptr;
    juce::XmlElement* dataList = nullptr;
    
    juce::Array <juce::Array <juce::String>> *componentsArray;

    //==============================================================================
    class SelectionColumnCustomComponent    : public Component
    {
    public:
        SelectionColumnCustomComponent (componentTable& td)
            : owner (td)
        {
            addAndMakeVisible (toggleButton);

            toggleButton.onClick = [this] {
                owner.setSelection (row, toggleButton.getToggleState());
            };
            
            //toggleButton.onClick = [this] { owner.componentsArray->remove(row); };
        }

        void resized() override
        {
            toggleButton.setBoundsInset (juce::BorderSize<int> (2));
        }

        void setRowAndColumn (int newRow, int newColumn)
        {
            row = newRow;
            columnId = newColumn;
            toggleButton.setToggleState (owner.getSelection(row)=="ON", juce::dontSendNotification);
        }

    private:
        componentTable& owner;
        juce::ToggleButton toggleButton;
        int row, columnId;
    };

    //==============================================================================
    /*class TutorialDataSorter
    {
    public:
        TutorialDataSorter (const juce::String& attributeToSortBy, bool forwards)
            : attributeToSort (attributeToSortBy),
              direction (forwards ? 1 : -1)
        {}

        int compareElements (juce::XmlElement* first, juce::XmlElement* second) const
        {
            auto result = first->getStringAttribute (attributeToSort)
                                .compareNatural (second->getStringAttribute (attributeToSort));

            if (result == 0)
                result = first->getStringAttribute ("ID")
                               .compareNatural (second->getStringAttribute ("ID"));

            return direction * result;
        }

    private:
        juce::String attributeToSort;
        int direction;
    };*/

   

    juce::String getAttributeNameForColumnId (const int columnId) const
    {
        for (auto* columnXml : columnList->getChildIterator())
        {
            if (columnXml->getIntAttribute ("columnId") == columnId)
                return columnXml->getStringAttribute ("name");
        }

        return {};
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (componentTable)
};
