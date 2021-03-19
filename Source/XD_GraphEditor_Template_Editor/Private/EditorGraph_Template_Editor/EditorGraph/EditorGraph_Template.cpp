﻿// Fill out your copyright notice in the Description page of Project Settings.
#include "EditorGraph_Template_Editor/EditorGraph/EditorGraph_Template.h"
#include <EdGraph/EdGraphPin.h>

#include "EditorGraph_Template/Nodes/BP_GraphNode_Template.h"
#include "EditorGraph_Template_Editor/EditorGraph/EditorNodes/Editor_GraphNode_Template.h"
#include "EditorGraph_Template_Editor/Utility/GraphEditor_Template_Log.h"
#include "EditorGraph_Template/Graphs/BP_Graph_Template.h"

bool UEditorGraph_Template::IsNameUnique(const FText & InName)
{
	bool bUnique = true;
	for (UEdGraphNode* Node : Nodes)
	{
		UEditor_GraphNode_Template* PNode = Cast<UEditor_GraphNode_Template>(Node);
		FText NodeName = PNode->GetEdNodeName();
		if (bUnique==true && !NodeName.IsEmpty())
		{
			if (NodeName.EqualToCaseIgnored(InName))
			{
				GraphEditor_Template_Warning_Log("I found another node with the same name: %s.", *NodeName.ToString());
				bUnique = false;
			}
		}
	}
	return bUnique;
}

void UEditorGraph_Template::SaveGraph()
{
	LinkAssetNodes();
	MapNamedNodes();
}

void UEditorGraph_Template::ClearOldLinks()
{
	for (UEdGraphNode* EditorNode : Nodes)
	{
		UEditor_GraphNode_Template* EdNode = Cast<UEditor_GraphNode_Template>(EditorNode);
		if (EdNode)
			EdNode->AssetNode->ClearLinks();
	}
}

void UEditorGraph_Template::LinkAssetNodes()
{
	ClearOldLinks();
	GraphEditor_Template_Log("Starting to link all asset nodes from the editor graph links.");
	for (UEdGraphNode* EditorNode : Nodes)
	{
		if (UEditor_GraphNode_Template* EdNode = Cast<UEditor_GraphNode_Template>(EditorNode))
		{
			UBP_GraphNode_Template* NodeAsset = EdNode->AssetNode;
			if (NodeAsset != nullptr)
			{

				TArray<UEdGraphPin*>& EdPinsParent = EdNode->Pins;
				TArray<UEdGraphNode*>Children;

				for (UEdGraphPin* Pin : EdPinsParent)
				{
					//Take only the output pins
					if (Pin->Direction == EEdGraphPinDirection::EGPD_Output)
					{

						TArray<UEdGraphPin*>& EdPinsChildren = Pin->LinkedTo;
						for (UEdGraphPin* LinkedPin : EdPinsChildren)
						    Children.Add(LinkedPin->GetOwningNode());
	
					}

				}

                EdNode->SaveNodesAsChildren(Children);
			}
			else
			{
				GraphEditor_Template_Error_Log("There is no asset node linked to this editor node.");
			}
		}
		else 
		{
			GraphEditor_Template_Warning_Log("An unknow EdNode has been found.");
		}
	}

}

void UEditorGraph_Template::RefreshNodes()
{
	for (UEdGraphNode* Node : Nodes)
	{
		if (UEditor_GraphNode_Template* BP_GraphNode_Template = Cast<UEditor_GraphNode_Template>(Node))
			BP_GraphNode_Template->UpdateVisualNode();
	}
}

void UEditorGraph_Template::MapNamedNodes()
{
	UBP_Graph_Template* Graph = GetGraphAsset();
	Graph->NamedNodes.Empty();
    Graph->NodesNames.Empty();

	for (UEdGraphNode* Node : Nodes)
	{
		if (UEditor_GraphNode_Template* BP_GraphNode_Template = Cast<UEditor_GraphNode_Template>(Node))
		{
			FText Name = BP_GraphNode_Template->GetEdNodeName();
            if (!Name.IsEmpty())
            {
                Graph->NamedNodes.Add(Name.ToString(), BP_GraphNode_Template->AssetNode);
                Graph->NodesNames.Add(BP_GraphNode_Template->AssetNode, Name.ToString());
            }
		}
	}
}

UBP_Graph_Template * UEditorGraph_Template::GetGraphAsset()
{
	return Cast<UBP_Graph_Template>(GetOuter());
}
