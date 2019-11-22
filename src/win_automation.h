#pragma warning( disable: 4049 )  /* more than 64k source lines */
    
/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __uiautomationcore_h__
#define __uiautomationcore_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IUIAutomationElement_FWD_DEFINED__
#define __IUIAutomationElement_FWD_DEFINED__
typedef interface IUIAutomationElement IUIAutomationElement;

#endif 	/* __IUIAutomationElement_FWD_DEFINED__ */


#ifndef __IUIAutomationCondition_FWD_DEFINED__
#define __IUIAutomationCondition_FWD_DEFINED__
typedef interface IUIAutomationCondition IUIAutomationCondition;

#endif 	/* __IUIAutomationCondition_FWD_DEFINED__ */


#ifndef __IUIAutomationElementArray_FWD_DEFINED__
#define __IUIAutomationElementArray_FWD_DEFINED__
typedef interface IUIAutomationElementArray IUIAutomationElementArray;

#endif 	/* __IUIAutomationElementArray_FWD_DEFINED__ */


#ifndef __IUIAutomationCacheRequest_FWD_DEFINED__
#define __IUIAutomationCacheRequest_FWD_DEFINED__
typedef interface IUIAutomationCacheRequest IUIAutomationCacheRequest;

#endif 	/* __IUIAutomationCacheRequest_FWD_DEFINED__ */


#ifndef __IUIAutomationBoolCondition_FWD_DEFINED__
#define __IUIAutomationBoolCondition_FWD_DEFINED__
typedef interface IUIAutomationBoolCondition IUIAutomationBoolCondition;

#endif 	/* __IUIAutomationBoolCondition_FWD_DEFINED__ */


#ifndef __IUIAutomationPropertyCondition_FWD_DEFINED__
#define __IUIAutomationPropertyCondition_FWD_DEFINED__
typedef interface IUIAutomationPropertyCondition IUIAutomationPropertyCondition;

#endif 	/* __IUIAutomationPropertyCondition_FWD_DEFINED__ */


#ifndef __IUIAutomationAndCondition_FWD_DEFINED__
#define __IUIAutomationAndCondition_FWD_DEFINED__
typedef interface IUIAutomationAndCondition IUIAutomationAndCondition;

#endif 	/* __IUIAutomationAndCondition_FWD_DEFINED__ */


#ifndef __IUIAutomationOrCondition_FWD_DEFINED__
#define __IUIAutomationOrCondition_FWD_DEFINED__
typedef interface IUIAutomationOrCondition IUIAutomationOrCondition;

#endif 	/* __IUIAutomationOrCondition_FWD_DEFINED__ */


#ifndef __IUIAutomationNotCondition_FWD_DEFINED__
#define __IUIAutomationNotCondition_FWD_DEFINED__
typedef interface IUIAutomationNotCondition IUIAutomationNotCondition;

#endif 	/* __IUIAutomationNotCondition_FWD_DEFINED__ */


#ifndef __IUIAutomationTreeWalker_FWD_DEFINED__
#define __IUIAutomationTreeWalker_FWD_DEFINED__
typedef interface IUIAutomationTreeWalker IUIAutomationTreeWalker;

#endif 	/* __IUIAutomationTreeWalker_FWD_DEFINED__ */


#ifndef __IUIAutomationEventHandler_FWD_DEFINED__
#define __IUIAutomationEventHandler_FWD_DEFINED__
typedef interface IUIAutomationEventHandler IUIAutomationEventHandler;

#endif 	/* __IUIAutomationEventHandler_FWD_DEFINED__ */


#ifndef __IUIAutomationPropertyChangedEventHandler_FWD_DEFINED__
#define __IUIAutomationPropertyChangedEventHandler_FWD_DEFINED__
typedef interface IUIAutomationPropertyChangedEventHandler IUIAutomationPropertyChangedEventHandler;

#endif 	/* __IUIAutomationPropertyChangedEventHandler_FWD_DEFINED__ */


#ifndef __IUIAutomationStructureChangedEventHandler_FWD_DEFINED__
#define __IUIAutomationStructureChangedEventHandler_FWD_DEFINED__
typedef interface IUIAutomationStructureChangedEventHandler IUIAutomationStructureChangedEventHandler;

#endif 	/* __IUIAutomationStructureChangedEventHandler_FWD_DEFINED__ */


#ifndef __IUIAutomationFocusChangedEventHandler_FWD_DEFINED__
#define __IUIAutomationFocusChangedEventHandler_FWD_DEFINED__
typedef interface IUIAutomationFocusChangedEventHandler IUIAutomationFocusChangedEventHandler;

#endif 	/* __IUIAutomationFocusChangedEventHandler_FWD_DEFINED__ */


#ifndef __IUIAutomationInvokePattern_FWD_DEFINED__
#define __IUIAutomationInvokePattern_FWD_DEFINED__
typedef interface IUIAutomationInvokePattern IUIAutomationInvokePattern;

#endif 	/* __IUIAutomationInvokePattern_FWD_DEFINED__ */


#ifndef __IUIAutomationDockPattern_FWD_DEFINED__
#define __IUIAutomationDockPattern_FWD_DEFINED__
typedef interface IUIAutomationDockPattern IUIAutomationDockPattern;

#endif 	/* __IUIAutomationDockPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationExpandCollapsePattern_FWD_DEFINED__
#define __IUIAutomationExpandCollapsePattern_FWD_DEFINED__
typedef interface IUIAutomationExpandCollapsePattern IUIAutomationExpandCollapsePattern;

#endif 	/* __IUIAutomationExpandCollapsePattern_FWD_DEFINED__ */


#ifndef __IUIAutomationGridPattern_FWD_DEFINED__
#define __IUIAutomationGridPattern_FWD_DEFINED__
typedef interface IUIAutomationGridPattern IUIAutomationGridPattern;

#endif 	/* __IUIAutomationGridPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationGridItemPattern_FWD_DEFINED__
#define __IUIAutomationGridItemPattern_FWD_DEFINED__
typedef interface IUIAutomationGridItemPattern IUIAutomationGridItemPattern;

#endif 	/* __IUIAutomationGridItemPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationMultipleViewPattern_FWD_DEFINED__
#define __IUIAutomationMultipleViewPattern_FWD_DEFINED__
typedef interface IUIAutomationMultipleViewPattern IUIAutomationMultipleViewPattern;

#endif 	/* __IUIAutomationMultipleViewPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationRangeValuePattern_FWD_DEFINED__
#define __IUIAutomationRangeValuePattern_FWD_DEFINED__
typedef interface IUIAutomationRangeValuePattern IUIAutomationRangeValuePattern;

#endif 	/* __IUIAutomationRangeValuePattern_FWD_DEFINED__ */


#ifndef __IUIAutomationScrollPattern_FWD_DEFINED__
#define __IUIAutomationScrollPattern_FWD_DEFINED__
typedef interface IUIAutomationScrollPattern IUIAutomationScrollPattern;

#endif 	/* __IUIAutomationScrollPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationScrollItemPattern_FWD_DEFINED__
#define __IUIAutomationScrollItemPattern_FWD_DEFINED__
typedef interface IUIAutomationScrollItemPattern IUIAutomationScrollItemPattern;

#endif 	/* __IUIAutomationScrollItemPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationSelectionPattern_FWD_DEFINED__
#define __IUIAutomationSelectionPattern_FWD_DEFINED__
typedef interface IUIAutomationSelectionPattern IUIAutomationSelectionPattern;

#endif 	/* __IUIAutomationSelectionPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationSelectionItemPattern_FWD_DEFINED__
#define __IUIAutomationSelectionItemPattern_FWD_DEFINED__
typedef interface IUIAutomationSelectionItemPattern IUIAutomationSelectionItemPattern;

#endif 	/* __IUIAutomationSelectionItemPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationSynchronizedInputPattern_FWD_DEFINED__
#define __IUIAutomationSynchronizedInputPattern_FWD_DEFINED__
typedef interface IUIAutomationSynchronizedInputPattern IUIAutomationSynchronizedInputPattern;

#endif 	/* __IUIAutomationSynchronizedInputPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationTablePattern_FWD_DEFINED__
#define __IUIAutomationTablePattern_FWD_DEFINED__
typedef interface IUIAutomationTablePattern IUIAutomationTablePattern;

#endif 	/* __IUIAutomationTablePattern_FWD_DEFINED__ */


#ifndef __IUIAutomationTableItemPattern_FWD_DEFINED__
#define __IUIAutomationTableItemPattern_FWD_DEFINED__
typedef interface IUIAutomationTableItemPattern IUIAutomationTableItemPattern;

#endif 	/* __IUIAutomationTableItemPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationTogglePattern_FWD_DEFINED__
#define __IUIAutomationTogglePattern_FWD_DEFINED__
typedef interface IUIAutomationTogglePattern IUIAutomationTogglePattern;

#endif 	/* __IUIAutomationTogglePattern_FWD_DEFINED__ */


#ifndef __IUIAutomationTransformPattern_FWD_DEFINED__
#define __IUIAutomationTransformPattern_FWD_DEFINED__
typedef interface IUIAutomationTransformPattern IUIAutomationTransformPattern;

#endif 	/* __IUIAutomationTransformPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationValuePattern_FWD_DEFINED__
#define __IUIAutomationValuePattern_FWD_DEFINED__
typedef interface IUIAutomationValuePattern IUIAutomationValuePattern;

#endif 	/* __IUIAutomationValuePattern_FWD_DEFINED__ */


#ifndef __IUIAutomationWindowPattern_FWD_DEFINED__
#define __IUIAutomationWindowPattern_FWD_DEFINED__
typedef interface IUIAutomationWindowPattern IUIAutomationWindowPattern;

#endif 	/* __IUIAutomationWindowPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationTextRange_FWD_DEFINED__
#define __IUIAutomationTextRange_FWD_DEFINED__
typedef interface IUIAutomationTextRange IUIAutomationTextRange;

#endif 	/* __IUIAutomationTextRange_FWD_DEFINED__ */


#ifndef __IUIAutomationTextRangeArray_FWD_DEFINED__
#define __IUIAutomationTextRangeArray_FWD_DEFINED__
typedef interface IUIAutomationTextRangeArray IUIAutomationTextRangeArray;

#endif 	/* __IUIAutomationTextRangeArray_FWD_DEFINED__ */


#ifndef __IUIAutomationTextPattern_FWD_DEFINED__
#define __IUIAutomationTextPattern_FWD_DEFINED__
typedef interface IUIAutomationTextPattern IUIAutomationTextPattern;

#endif 	/* __IUIAutomationTextPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationLegacyIAccessiblePattern_FWD_DEFINED__
#define __IUIAutomationLegacyIAccessiblePattern_FWD_DEFINED__
typedef interface IUIAutomationLegacyIAccessiblePattern IUIAutomationLegacyIAccessiblePattern;

#endif 	/* __IUIAutomationLegacyIAccessiblePattern_FWD_DEFINED__ */


#ifndef __IAccessible_FWD_DEFINED__
#define __IAccessible_FWD_DEFINED__
typedef interface IAccessible IAccessible;

#endif 	/* __IAccessible_FWD_DEFINED__ */


#ifndef __IUIAutomationItemContainerPattern_FWD_DEFINED__
#define __IUIAutomationItemContainerPattern_FWD_DEFINED__
typedef interface IUIAutomationItemContainerPattern IUIAutomationItemContainerPattern;

#endif 	/* __IUIAutomationItemContainerPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationVirtualizedItemPattern_FWD_DEFINED__
#define __IUIAutomationVirtualizedItemPattern_FWD_DEFINED__
typedef interface IUIAutomationVirtualizedItemPattern IUIAutomationVirtualizedItemPattern;

#endif 	/* __IUIAutomationVirtualizedItemPattern_FWD_DEFINED__ */


#ifndef __IUIAutomationProxyFactory_FWD_DEFINED__
#define __IUIAutomationProxyFactory_FWD_DEFINED__
typedef interface IUIAutomationProxyFactory IUIAutomationProxyFactory;

#endif 	/* __IUIAutomationProxyFactory_FWD_DEFINED__ */


#ifndef __IRawElementProviderSimple_FWD_DEFINED__
#define __IRawElementProviderSimple_FWD_DEFINED__
typedef interface IRawElementProviderSimple IRawElementProviderSimple;

#endif 	/* __IRawElementProviderSimple_FWD_DEFINED__ */


#ifndef __IUIAutomationProxyFactoryEntry_FWD_DEFINED__
#define __IUIAutomationProxyFactoryEntry_FWD_DEFINED__
typedef interface IUIAutomationProxyFactoryEntry IUIAutomationProxyFactoryEntry;

#endif 	/* __IUIAutomationProxyFactoryEntry_FWD_DEFINED__ */


#ifndef __IUIAutomationProxyFactoryMapping_FWD_DEFINED__
#define __IUIAutomationProxyFactoryMapping_FWD_DEFINED__
typedef interface IUIAutomationProxyFactoryMapping IUIAutomationProxyFactoryMapping;

#endif 	/* __IUIAutomationProxyFactoryMapping_FWD_DEFINED__ */


#ifndef __IUIAutomation_FWD_DEFINED__
#define __IUIAutomation_FWD_DEFINED__
typedef interface IUIAutomation IUIAutomation;

#endif 	/* __IUIAutomation_FWD_DEFINED__ */


#ifndef __CUIAutomation_FWD_DEFINED__
#define __CUIAutomation_FWD_DEFINED__

#ifdef __cplusplus
typedef class CUIAutomation CUIAutomation;
#else
typedef struct CUIAutomation CUIAutomation;
#endif /* __cplusplus */

#endif 	/* __CUIAutomation_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __UIAutomationClient_LIBRARY_DEFINED__
#define __UIAutomationClient_LIBRARY_DEFINED__

/* library UIAutomationClient */
/* [version][uuid] */ 













































typedef /* [public][public][public][public][public][public][public][public][public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0004_0001
    {
        TreeScope_Element	= 1,
        TreeScope_Children	= 2,
        TreeScope_Descendants	= 4,
        TreeScope_Parent	= 8,
        TreeScope_Ancestors	= 16,
        TreeScope_Subtree	= 7
    } 	TreeScope;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0004_0002
    {
        AutomationElementMode_None	= 0,
        AutomationElementMode_Full	= 1
    } 	AutomationElementMode;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0004_0003
    {
        OrientationType_None	= 0,
        OrientationType_Horizontal	= 1,
        OrientationType_Vertical	= 2
    } 	OrientationType;

typedef struct tagtagRECT
    {
    long left;
    long top;
    long right;
    long bottom;
    } 	tagRECT;

typedef struct tagtagPOINT
    {
    long x;
    long y;
    } 	tagPOINT;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0004_0004
    {
        PropertyConditionFlags_None	= 0,
        PropertyConditionFlags_IgnoreCase	= 1
    } 	PropertyConditionFlags;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0016_0001
    {
        StructureChangeType_ChildAdded	= 0,
        StructureChangeType_ChildRemoved	= 1,
        StructureChangeType_ChildrenInvalidated	= 2,
        StructureChangeType_ChildrenBulkAdded	= 3,
        StructureChangeType_ChildrenBulkRemoved	= 4,
        StructureChangeType_ChildrenReordered	= 5
    } 	StructureChangeType;

typedef /* [public][public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0019_0001
    {
        DockPosition_Top	= 0,
        DockPosition_Left	= 1,
        DockPosition_Bottom	= 2,
        DockPosition_Right	= 3,
        DockPosition_Fill	= 4,
        DockPosition_None	= 5
    } 	DockPosition;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0020_0001
    {
        ExpandCollapseState_Collapsed	= 0,
        ExpandCollapseState_Expanded	= 1,
        ExpandCollapseState_PartiallyExpanded	= 2,
        ExpandCollapseState_LeafNode	= 3
    } 	ExpandCollapseState;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0025_0001
    {
        ScrollAmount_LargeDecrement	= 0,
        ScrollAmount_SmallDecrement	= 1,
        ScrollAmount_NoAmount	= 2,
        ScrollAmount_LargeIncrement	= 3,
        ScrollAmount_SmallIncrement	= 4
    } 	ScrollAmount;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0029_0001
    {
        SynchronizedInputType_KeyUp	= 1,
        SynchronizedInputType_KeyDown	= 2,
        SynchronizedInputType_LeftMouseUp	= 4,
        SynchronizedInputType_LeftMouseDown	= 8,
        SynchronizedInputType_RightMouseUp	= 16,
        SynchronizedInputType_RightMouseDown	= 32
    } 	SynchronizedInputType;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0029_0002
    {
        RowOrColumnMajor_RowMajor	= 0,
        RowOrColumnMajor_ColumnMajor	= 1,
        RowOrColumnMajor_Indeterminate	= 2
    } 	RowOrColumnMajor;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0032_0001
    {
        ToggleState_Off	= 0,
        ToggleState_On	= 1,
        ToggleState_Indeterminate	= 2
    } 	ToggleState;

typedef /* [public][public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0032_0002
    {
        WindowVisualState_Normal	= 0,
        WindowVisualState_Maximized	= 1,
        WindowVisualState_Minimized	= 2
    } 	WindowVisualState;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0032_0003
    {
        WindowInteractionState_Running	= 0,
        WindowInteractionState_Closing	= 1,
        WindowInteractionState_ReadyForUserInteraction	= 2,
        WindowInteractionState_BlockedByModalWindow	= 3,
        WindowInteractionState_NotResponding	= 4
    } 	WindowInteractionState;

typedef /* [public][public][public][public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0036_0001
    {
        TextPatternRangeEndpoint_Start	= 0,
        TextPatternRangeEndpoint_End	= 1
    } 	TextPatternRangeEndpoint;

typedef /* [public][public][public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0036_0002
    {
        TextUnit_Character	= 0,
        TextUnit_Format	= 1,
        TextUnit_Word	= 2,
        TextUnit_Line	= 3,
        TextUnit_Paragraph	= 4,
        TextUnit_Page	= 5,
        TextUnit_Document	= 6
    } 	TextUnit;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0038_0001
    {
        SupportedTextSelection_None	= 0,
        SupportedTextSelection_Single	= 1,
        SupportedTextSelection_Multiple	= 2
    } 	SupportedTextSelection;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_UIAutomationCore_0001_0044_0001
    {
        ProviderOptions_ClientSideProvider	= 1,
        ProviderOptions_ServerSideProvider	= 2,
        ProviderOptions_NonClientAreaProvider	= 4,
        ProviderOptions_OverrideProvider	= 8,
        ProviderOptions_ProviderOwnsSetFocus	= 16,
        ProviderOptions_UseComThreading	= 32
    } 	ProviderOptions;


EXTERN_C const IID LIBID_UIAutomationClient;

#ifndef __IUIAutomationElement_INTERFACE_DEFINED__
#define __IUIAutomationElement_INTERFACE_DEFINED__

/* interface IUIAutomationElement */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationElement;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D22108AA-8AC5-49A5-837B-37BBB3D7591E")
    IUIAutomationElement : public IUnknown
    {
    public:
        virtual HRESULT __stdcall SetFocus( void) = 0;
        
        virtual HRESULT __stdcall GetRuntimeId( 
            /* [retval][out] */ SAFEARRAY * *runtimeId) = 0;
        
        virtual HRESULT __stdcall FindFirst( 
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCondition *condition,
            /* [retval][out] */ IUIAutomationElement **found) = 0;
        
        virtual HRESULT __stdcall FindAll( 
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCondition *condition,
            /* [retval][out] */ IUIAutomationElementArray **found) = 0;
        
        virtual HRESULT __stdcall FindFirstBuildCache( 
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCondition *condition,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **found) = 0;
        
        virtual HRESULT __stdcall FindAllBuildCache( 
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCondition *condition,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElementArray **found) = 0;
        
        virtual HRESULT __stdcall BuildUpdatedCache( 
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **updatedElement) = 0;
        
        virtual HRESULT __stdcall GetCurrentPropertyValue( 
            /* [in] */ int propertyId,
            /* [retval][out] */ VARIANT *retVal) = 0;
        
        virtual HRESULT __stdcall GetCurrentPropertyValueEx( 
            /* [in] */ int propertyId,
            /* [in] */ long ignoreDefaultValue,
            /* [retval][out] */ VARIANT *retVal) = 0;
        
        virtual HRESULT __stdcall GetCachedPropertyValue( 
            /* [in] */ int propertyId,
            /* [retval][out] */ VARIANT *retVal) = 0;
        
        virtual HRESULT __stdcall GetCachedPropertyValueEx( 
            /* [in] */ int propertyId,
            /* [in] */ long ignoreDefaultValue,
            /* [retval][out] */ VARIANT *retVal) = 0;
        
        virtual HRESULT __stdcall GetCurrentPatternAs( 
            /* [in] */ int patternId,
            /* [in] */ GUID *riid,
            /* [retval][out] */ void **patternObject) = 0;
        
        virtual HRESULT __stdcall GetCachedPatternAs( 
            /* [in] */ int patternId,
            /* [in] */ GUID *riid,
            /* [retval][out] */ void **patternObject) = 0;
        
        virtual HRESULT __stdcall GetCurrentPattern( 
            /* [in] */ int patternId,
            /* [retval][out] */ IUnknown **patternObject) = 0;
        
        virtual HRESULT __stdcall GetCachedPattern( 
            /* [in] */ int patternId,
            /* [retval][out] */ IUnknown **patternObject) = 0;
        
        virtual HRESULT __stdcall GetCachedParent( 
            /* [retval][out] */ IUIAutomationElement **parent) = 0;
        
        virtual HRESULT __stdcall GetCachedChildren( 
            /* [retval][out] */ IUIAutomationElementArray **children) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentProcessId( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentControlType( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentLocalizedControlType( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentName( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentAcceleratorKey( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentAccessKey( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentHasKeyboardFocus( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsKeyboardFocusable( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsEnabled( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentAutomationId( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentClassName( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentHelpText( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentCulture( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsControlElement( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsContentElement( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsPassword( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentNativeWindowHandle( 
            /* [retval][out] */ void **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentItemType( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsOffscreen( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentOrientation( 
            /* [retval][out] */ OrientationType *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentFrameworkId( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsRequiredForForm( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentItemStatus( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentBoundingRectangle( 
            /* [retval][out] */ tagRECT *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentLabeledBy( 
            /* [retval][out] */ IUIAutomationElement **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentAriaRole( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentAriaProperties( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsDataValidForForm( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentControllerFor( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentDescribedBy( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentFlowsTo( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentProviderDescription( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedProcessId( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedControlType( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedLocalizedControlType( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedName( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedAcceleratorKey( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedAccessKey( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedHasKeyboardFocus( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsKeyboardFocusable( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsEnabled( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedAutomationId( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedClassName( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedHelpText( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedCulture( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsControlElement( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsContentElement( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsPassword( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedNativeWindowHandle( 
            /* [retval][out] */ void **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedItemType( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsOffscreen( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedOrientation( 
            /* [retval][out] */ OrientationType *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedFrameworkId( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsRequiredForForm( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedItemStatus( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedBoundingRectangle( 
            /* [retval][out] */ tagRECT *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedLabeledBy( 
            /* [retval][out] */ IUIAutomationElement **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedAriaRole( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedAriaProperties( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsDataValidForForm( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedControllerFor( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedDescribedBy( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedFlowsTo( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedProviderDescription( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual HRESULT __stdcall GetClickablePoint( 
            /* [out] */ tagPOINT *clickable,
            /* [retval][out] */ long *gotClickable) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationElementVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationElement * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationElement * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationElement * This);
        
        HRESULT ( __stdcall *SetFocus )( 
            IUIAutomationElement * This);
        
        HRESULT ( __stdcall *GetRuntimeId )( 
            IUIAutomationElement * This,
            /* [retval][out] */ SAFEARRAY * *runtimeId);
        
        HRESULT ( __stdcall *FindFirst )( 
            IUIAutomationElement * This,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCondition *condition,
            /* [retval][out] */ IUIAutomationElement **found);
        
        HRESULT ( __stdcall *FindAll )( 
            IUIAutomationElement * This,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCondition *condition,
            /* [retval][out] */ IUIAutomationElementArray **found);
        
        HRESULT ( __stdcall *FindFirstBuildCache )( 
            IUIAutomationElement * This,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCondition *condition,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **found);
        
        HRESULT ( __stdcall *FindAllBuildCache )( 
            IUIAutomationElement * This,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCondition *condition,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElementArray **found);
        
        HRESULT ( __stdcall *BuildUpdatedCache )( 
            IUIAutomationElement * This,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **updatedElement);
        
        HRESULT ( __stdcall *GetCurrentPropertyValue )( 
            IUIAutomationElement * This,
            /* [in] */ int propertyId,
            /* [retval][out] */ VARIANT *retVal);
        
        HRESULT ( __stdcall *GetCurrentPropertyValueEx )( 
            IUIAutomationElement * This,
            /* [in] */ int propertyId,
            /* [in] */ long ignoreDefaultValue,
            /* [retval][out] */ VARIANT *retVal);
        
        HRESULT ( __stdcall *GetCachedPropertyValue )( 
            IUIAutomationElement * This,
            /* [in] */ int propertyId,
            /* [retval][out] */ VARIANT *retVal);
        
        HRESULT ( __stdcall *GetCachedPropertyValueEx )( 
            IUIAutomationElement * This,
            /* [in] */ int propertyId,
            /* [in] */ long ignoreDefaultValue,
            /* [retval][out] */ VARIANT *retVal);
        
        HRESULT ( __stdcall *GetCurrentPatternAs )( 
            IUIAutomationElement * This,
            /* [in] */ int patternId,
            /* [in] */ GUID *riid,
            /* [retval][out] */ void **patternObject);
        
        HRESULT ( __stdcall *GetCachedPatternAs )( 
            IUIAutomationElement * This,
            /* [in] */ int patternId,
            /* [in] */ GUID *riid,
            /* [retval][out] */ void **patternObject);
        
        HRESULT ( __stdcall *GetCurrentPattern )( 
            IUIAutomationElement * This,
            /* [in] */ int patternId,
            /* [retval][out] */ IUnknown **patternObject);
        
        HRESULT ( __stdcall *GetCachedPattern )( 
            IUIAutomationElement * This,
            /* [in] */ int patternId,
            /* [retval][out] */ IUnknown **patternObject);
        
        HRESULT ( __stdcall *GetCachedParent )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElement **parent);
        
        HRESULT ( __stdcall *GetCachedChildren )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElementArray **children);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentProcessId )( 
            IUIAutomationElement * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentControlType )( 
            IUIAutomationElement * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentLocalizedControlType )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentName )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentAcceleratorKey )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentAccessKey )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentHasKeyboardFocus )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsKeyboardFocusable )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsEnabled )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentAutomationId )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentClassName )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentHelpText )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentCulture )( 
            IUIAutomationElement * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsControlElement )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsContentElement )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsPassword )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentNativeWindowHandle )( 
            IUIAutomationElement * This,
            /* [retval][out] */ void **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentItemType )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsOffscreen )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentOrientation )( 
            IUIAutomationElement * This,
            /* [retval][out] */ OrientationType *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentFrameworkId )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsRequiredForForm )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentItemStatus )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentBoundingRectangle )( 
            IUIAutomationElement * This,
            /* [retval][out] */ tagRECT *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentLabeledBy )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElement **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentAriaRole )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentAriaProperties )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsDataValidForForm )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentControllerFor )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentDescribedBy )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentFlowsTo )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentProviderDescription )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedProcessId )( 
            IUIAutomationElement * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedControlType )( 
            IUIAutomationElement * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedLocalizedControlType )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedName )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedAcceleratorKey )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedAccessKey )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedHasKeyboardFocus )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsKeyboardFocusable )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsEnabled )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedAutomationId )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedClassName )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedHelpText )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedCulture )( 
            IUIAutomationElement * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsControlElement )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsContentElement )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsPassword )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedNativeWindowHandle )( 
            IUIAutomationElement * This,
            /* [retval][out] */ void **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedItemType )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsOffscreen )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedOrientation )( 
            IUIAutomationElement * This,
            /* [retval][out] */ OrientationType *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedFrameworkId )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsRequiredForForm )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedItemStatus )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedBoundingRectangle )( 
            IUIAutomationElement * This,
            /* [retval][out] */ tagRECT *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedLabeledBy )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElement **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedAriaRole )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedAriaProperties )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsDataValidForForm )( 
            IUIAutomationElement * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedControllerFor )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedDescribedBy )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedFlowsTo )( 
            IUIAutomationElement * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedProviderDescription )( 
            IUIAutomationElement * This,
            /* [retval][out] */ BSTR *retVal);
        
        HRESULT ( __stdcall *GetClickablePoint )( 
            IUIAutomationElement * This,
            /* [out] */ tagPOINT *clickable,
            /* [retval][out] */ long *gotClickable);
        
        END_INTERFACE
    } IUIAutomationElementVtbl;

    interface IUIAutomationElement
    {
        CONST_VTBL struct IUIAutomationElementVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationElement_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationElement_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationElement_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationElement_SetFocus(This)	\
    ( (This)->lpVtbl -> SetFocus(This) ) 

#define IUIAutomationElement_GetRuntimeId(This,runtimeId)	\
    ( (This)->lpVtbl -> GetRuntimeId(This,runtimeId) ) 

#define IUIAutomationElement_FindFirst(This,scope,condition,found)	\
    ( (This)->lpVtbl -> FindFirst(This,scope,condition,found) ) 

#define IUIAutomationElement_FindAll(This,scope,condition,found)	\
    ( (This)->lpVtbl -> FindAll(This,scope,condition,found) ) 

#define IUIAutomationElement_FindFirstBuildCache(This,scope,condition,cacheRequest,found)	\
    ( (This)->lpVtbl -> FindFirstBuildCache(This,scope,condition,cacheRequest,found) ) 

#define IUIAutomationElement_FindAllBuildCache(This,scope,condition,cacheRequest,found)	\
    ( (This)->lpVtbl -> FindAllBuildCache(This,scope,condition,cacheRequest,found) ) 

#define IUIAutomationElement_BuildUpdatedCache(This,cacheRequest,updatedElement)	\
    ( (This)->lpVtbl -> BuildUpdatedCache(This,cacheRequest,updatedElement) ) 

#define IUIAutomationElement_GetCurrentPropertyValue(This,propertyId,retVal)	\
    ( (This)->lpVtbl -> GetCurrentPropertyValue(This,propertyId,retVal) ) 

#define IUIAutomationElement_GetCurrentPropertyValueEx(This,propertyId,ignoreDefaultValue,retVal)	\
    ( (This)->lpVtbl -> GetCurrentPropertyValueEx(This,propertyId,ignoreDefaultValue,retVal) ) 

#define IUIAutomationElement_GetCachedPropertyValue(This,propertyId,retVal)	\
    ( (This)->lpVtbl -> GetCachedPropertyValue(This,propertyId,retVal) ) 

#define IUIAutomationElement_GetCachedPropertyValueEx(This,propertyId,ignoreDefaultValue,retVal)	\
    ( (This)->lpVtbl -> GetCachedPropertyValueEx(This,propertyId,ignoreDefaultValue,retVal) ) 

#define IUIAutomationElement_GetCurrentPatternAs(This,patternId,riid,patternObject)	\
    ( (This)->lpVtbl -> GetCurrentPatternAs(This,patternId,riid,patternObject) ) 

#define IUIAutomationElement_GetCachedPatternAs(This,patternId,riid,patternObject)	\
    ( (This)->lpVtbl -> GetCachedPatternAs(This,patternId,riid,patternObject) ) 

#define IUIAutomationElement_GetCurrentPattern(This,patternId,patternObject)	\
    ( (This)->lpVtbl -> GetCurrentPattern(This,patternId,patternObject) ) 

#define IUIAutomationElement_GetCachedPattern(This,patternId,patternObject)	\
    ( (This)->lpVtbl -> GetCachedPattern(This,patternId,patternObject) ) 

#define IUIAutomationElement_GetCachedParent(This,parent)	\
    ( (This)->lpVtbl -> GetCachedParent(This,parent) ) 

#define IUIAutomationElement_GetCachedChildren(This,children)	\
    ( (This)->lpVtbl -> GetCachedChildren(This,children) ) 

#define IUIAutomationElement_get_CurrentProcessId(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentProcessId(This,retVal) ) 

#define IUIAutomationElement_get_CurrentControlType(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentControlType(This,retVal) ) 

#define IUIAutomationElement_get_CurrentLocalizedControlType(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentLocalizedControlType(This,retVal) ) 

#define IUIAutomationElement_get_CurrentName(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentName(This,retVal) ) 

#define IUIAutomationElement_get_CurrentAcceleratorKey(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentAcceleratorKey(This,retVal) ) 

#define IUIAutomationElement_get_CurrentAccessKey(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentAccessKey(This,retVal) ) 

#define IUIAutomationElement_get_CurrentHasKeyboardFocus(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentHasKeyboardFocus(This,retVal) ) 

#define IUIAutomationElement_get_CurrentIsKeyboardFocusable(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsKeyboardFocusable(This,retVal) ) 

#define IUIAutomationElement_get_CurrentIsEnabled(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsEnabled(This,retVal) ) 

#define IUIAutomationElement_get_CurrentAutomationId(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentAutomationId(This,retVal) ) 

#define IUIAutomationElement_get_CurrentClassName(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentClassName(This,retVal) ) 

#define IUIAutomationElement_get_CurrentHelpText(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentHelpText(This,retVal) ) 

#define IUIAutomationElement_get_CurrentCulture(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentCulture(This,retVal) ) 

#define IUIAutomationElement_get_CurrentIsControlElement(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsControlElement(This,retVal) ) 

#define IUIAutomationElement_get_CurrentIsContentElement(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsContentElement(This,retVal) ) 

#define IUIAutomationElement_get_CurrentIsPassword(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsPassword(This,retVal) ) 

#define IUIAutomationElement_get_CurrentNativeWindowHandle(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentNativeWindowHandle(This,retVal) ) 

#define IUIAutomationElement_get_CurrentItemType(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentItemType(This,retVal) ) 

#define IUIAutomationElement_get_CurrentIsOffscreen(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsOffscreen(This,retVal) ) 

#define IUIAutomationElement_get_CurrentOrientation(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentOrientation(This,retVal) ) 

#define IUIAutomationElement_get_CurrentFrameworkId(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentFrameworkId(This,retVal) ) 

#define IUIAutomationElement_get_CurrentIsRequiredForForm(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsRequiredForForm(This,retVal) ) 

#define IUIAutomationElement_get_CurrentItemStatus(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentItemStatus(This,retVal) ) 

#define IUIAutomationElement_get_CurrentBoundingRectangle(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentBoundingRectangle(This,retVal) ) 

#define IUIAutomationElement_get_CurrentLabeledBy(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentLabeledBy(This,retVal) ) 

#define IUIAutomationElement_get_CurrentAriaRole(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentAriaRole(This,retVal) ) 

#define IUIAutomationElement_get_CurrentAriaProperties(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentAriaProperties(This,retVal) ) 

#define IUIAutomationElement_get_CurrentIsDataValidForForm(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsDataValidForForm(This,retVal) ) 

#define IUIAutomationElement_get_CurrentControllerFor(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentControllerFor(This,retVal) ) 

#define IUIAutomationElement_get_CurrentDescribedBy(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentDescribedBy(This,retVal) ) 

#define IUIAutomationElement_get_CurrentFlowsTo(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentFlowsTo(This,retVal) ) 

#define IUIAutomationElement_get_CurrentProviderDescription(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentProviderDescription(This,retVal) ) 

#define IUIAutomationElement_get_CachedProcessId(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedProcessId(This,retVal) ) 

#define IUIAutomationElement_get_CachedControlType(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedControlType(This,retVal) ) 

#define IUIAutomationElement_get_CachedLocalizedControlType(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedLocalizedControlType(This,retVal) ) 

#define IUIAutomationElement_get_CachedName(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedName(This,retVal) ) 

#define IUIAutomationElement_get_CachedAcceleratorKey(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedAcceleratorKey(This,retVal) ) 

#define IUIAutomationElement_get_CachedAccessKey(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedAccessKey(This,retVal) ) 

#define IUIAutomationElement_get_CachedHasKeyboardFocus(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedHasKeyboardFocus(This,retVal) ) 

#define IUIAutomationElement_get_CachedIsKeyboardFocusable(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsKeyboardFocusable(This,retVal) ) 

#define IUIAutomationElement_get_CachedIsEnabled(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsEnabled(This,retVal) ) 

#define IUIAutomationElement_get_CachedAutomationId(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedAutomationId(This,retVal) ) 

#define IUIAutomationElement_get_CachedClassName(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedClassName(This,retVal) ) 

#define IUIAutomationElement_get_CachedHelpText(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedHelpText(This,retVal) ) 

#define IUIAutomationElement_get_CachedCulture(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedCulture(This,retVal) ) 

#define IUIAutomationElement_get_CachedIsControlElement(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsControlElement(This,retVal) ) 

#define IUIAutomationElement_get_CachedIsContentElement(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsContentElement(This,retVal) ) 

#define IUIAutomationElement_get_CachedIsPassword(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsPassword(This,retVal) ) 

#define IUIAutomationElement_get_CachedNativeWindowHandle(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedNativeWindowHandle(This,retVal) ) 

#define IUIAutomationElement_get_CachedItemType(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedItemType(This,retVal) ) 

#define IUIAutomationElement_get_CachedIsOffscreen(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsOffscreen(This,retVal) ) 

#define IUIAutomationElement_get_CachedOrientation(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedOrientation(This,retVal) ) 

#define IUIAutomationElement_get_CachedFrameworkId(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedFrameworkId(This,retVal) ) 

#define IUIAutomationElement_get_CachedIsRequiredForForm(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsRequiredForForm(This,retVal) ) 

#define IUIAutomationElement_get_CachedItemStatus(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedItemStatus(This,retVal) ) 

#define IUIAutomationElement_get_CachedBoundingRectangle(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedBoundingRectangle(This,retVal) ) 

#define IUIAutomationElement_get_CachedLabeledBy(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedLabeledBy(This,retVal) ) 

#define IUIAutomationElement_get_CachedAriaRole(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedAriaRole(This,retVal) ) 

#define IUIAutomationElement_get_CachedAriaProperties(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedAriaProperties(This,retVal) ) 

#define IUIAutomationElement_get_CachedIsDataValidForForm(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsDataValidForForm(This,retVal) ) 

#define IUIAutomationElement_get_CachedControllerFor(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedControllerFor(This,retVal) ) 

#define IUIAutomationElement_get_CachedDescribedBy(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedDescribedBy(This,retVal) ) 

#define IUIAutomationElement_get_CachedFlowsTo(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedFlowsTo(This,retVal) ) 

#define IUIAutomationElement_get_CachedProviderDescription(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedProviderDescription(This,retVal) ) 

#define IUIAutomationElement_GetClickablePoint(This,clickable,gotClickable)	\
    ( (This)->lpVtbl -> GetClickablePoint(This,clickable,gotClickable) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationElement_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationCondition_INTERFACE_DEFINED__
#define __IUIAutomationCondition_INTERFACE_DEFINED__

/* interface IUIAutomationCondition */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationCondition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("352FFBA8-0973-437C-A61F-F64CAFD81DF9")
    IUIAutomationCondition : public IUnknown
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationConditionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationCondition * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationCondition * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationCondition * This);
        
        END_INTERFACE
    } IUIAutomationConditionVtbl;

    interface IUIAutomationCondition
    {
        CONST_VTBL struct IUIAutomationConditionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationCondition_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationCondition_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationCondition_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationCondition_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationElementArray_INTERFACE_DEFINED__
#define __IUIAutomationElementArray_INTERFACE_DEFINED__

/* interface IUIAutomationElementArray */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationElementArray;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("14314595-B4BC-4055-95F2-58F2E42C9855")
    IUIAutomationElementArray : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_Length( 
            /* [retval][out] */ int *Length) = 0;
        
        virtual HRESULT __stdcall GetElement( 
            /* [in] */ int index,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationElementArrayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationElementArray * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationElementArray * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationElementArray * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_Length )( 
            IUIAutomationElementArray * This,
            /* [retval][out] */ int *Length);
        
        HRESULT ( __stdcall *GetElement )( 
            IUIAutomationElementArray * This,
            /* [in] */ int index,
            /* [retval][out] */ IUIAutomationElement **element);
        
        END_INTERFACE
    } IUIAutomationElementArrayVtbl;

    interface IUIAutomationElementArray
    {
        CONST_VTBL struct IUIAutomationElementArrayVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationElementArray_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationElementArray_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationElementArray_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationElementArray_get_Length(This,Length)	\
    ( (This)->lpVtbl -> get_Length(This,Length) ) 

#define IUIAutomationElementArray_GetElement(This,index,element)	\
    ( (This)->lpVtbl -> GetElement(This,index,element) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationElementArray_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationCacheRequest_INTERFACE_DEFINED__
#define __IUIAutomationCacheRequest_INTERFACE_DEFINED__

/* interface IUIAutomationCacheRequest */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationCacheRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B32A92B5-BC25-4078-9C08-D7EE95C48E03")
    IUIAutomationCacheRequest : public IUnknown
    {
    public:
        virtual HRESULT __stdcall AddProperty( 
            /* [in] */ int propertyId) = 0;
        
        virtual HRESULT __stdcall AddPattern( 
            /* [in] */ int patternId) = 0;
        
        virtual HRESULT __stdcall Clone( 
            /* [retval][out] */ IUIAutomationCacheRequest **clonedRequest) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_TreeScope( 
            /* [retval][out] */ TreeScope *scope) = 0;
        
        virtual /* [propput] */ HRESULT __stdcall put_TreeScope( 
            /* [in] */ TreeScope scope) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_TreeFilter( 
            /* [retval][out] */ IUIAutomationCondition **filter) = 0;
        
        virtual /* [propput] */ HRESULT __stdcall put_TreeFilter( 
            /* [in] */ IUIAutomationCondition *filter) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_AutomationElementMode( 
            /* [retval][out] */ AutomationElementMode *mode) = 0;
        
        virtual /* [propput] */ HRESULT __stdcall put_AutomationElementMode( 
            /* [in] */ AutomationElementMode mode) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationCacheRequestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationCacheRequest * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationCacheRequest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationCacheRequest * This);
        
        HRESULT ( __stdcall *AddProperty )( 
            IUIAutomationCacheRequest * This,
            /* [in] */ int propertyId);
        
        HRESULT ( __stdcall *AddPattern )( 
            IUIAutomationCacheRequest * This,
            /* [in] */ int patternId);
        
        HRESULT ( __stdcall *Clone )( 
            IUIAutomationCacheRequest * This,
            /* [retval][out] */ IUIAutomationCacheRequest **clonedRequest);
        
        /* [propget] */ HRESULT ( __stdcall *get_TreeScope )( 
            IUIAutomationCacheRequest * This,
            /* [retval][out] */ TreeScope *scope);
        
        /* [propput] */ HRESULT ( __stdcall *put_TreeScope )( 
            IUIAutomationCacheRequest * This,
            /* [in] */ TreeScope scope);
        
        /* [propget] */ HRESULT ( __stdcall *get_TreeFilter )( 
            IUIAutomationCacheRequest * This,
            /* [retval][out] */ IUIAutomationCondition **filter);
        
        /* [propput] */ HRESULT ( __stdcall *put_TreeFilter )( 
            IUIAutomationCacheRequest * This,
            /* [in] */ IUIAutomationCondition *filter);
        
        /* [propget] */ HRESULT ( __stdcall *get_AutomationElementMode )( 
            IUIAutomationCacheRequest * This,
            /* [retval][out] */ AutomationElementMode *mode);
        
        /* [propput] */ HRESULT ( __stdcall *put_AutomationElementMode )( 
            IUIAutomationCacheRequest * This,
            /* [in] */ AutomationElementMode mode);
        
        END_INTERFACE
    } IUIAutomationCacheRequestVtbl;

    interface IUIAutomationCacheRequest
    {
        CONST_VTBL struct IUIAutomationCacheRequestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationCacheRequest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationCacheRequest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationCacheRequest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationCacheRequest_AddProperty(This,propertyId)	\
    ( (This)->lpVtbl -> AddProperty(This,propertyId) ) 

#define IUIAutomationCacheRequest_AddPattern(This,patternId)	\
    ( (This)->lpVtbl -> AddPattern(This,patternId) ) 

#define IUIAutomationCacheRequest_Clone(This,clonedRequest)	\
    ( (This)->lpVtbl -> Clone(This,clonedRequest) ) 

#define IUIAutomationCacheRequest_get_TreeScope(This,scope)	\
    ( (This)->lpVtbl -> get_TreeScope(This,scope) ) 

#define IUIAutomationCacheRequest_put_TreeScope(This,scope)	\
    ( (This)->lpVtbl -> put_TreeScope(This,scope) ) 

#define IUIAutomationCacheRequest_get_TreeFilter(This,filter)	\
    ( (This)->lpVtbl -> get_TreeFilter(This,filter) ) 

#define IUIAutomationCacheRequest_put_TreeFilter(This,filter)	\
    ( (This)->lpVtbl -> put_TreeFilter(This,filter) ) 

#define IUIAutomationCacheRequest_get_AutomationElementMode(This,mode)	\
    ( (This)->lpVtbl -> get_AutomationElementMode(This,mode) ) 

#define IUIAutomationCacheRequest_put_AutomationElementMode(This,mode)	\
    ( (This)->lpVtbl -> put_AutomationElementMode(This,mode) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationCacheRequest_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationBoolCondition_INTERFACE_DEFINED__
#define __IUIAutomationBoolCondition_INTERFACE_DEFINED__

/* interface IUIAutomationBoolCondition */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationBoolCondition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1B4E1F2E-75EB-4D0B-8952-5A69988E2307")
    IUIAutomationBoolCondition : public IUIAutomationCondition
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_BooleanValue( 
            /* [retval][out] */ long *boolVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationBoolConditionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationBoolCondition * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationBoolCondition * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationBoolCondition * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_BooleanValue )( 
            IUIAutomationBoolCondition * This,
            /* [retval][out] */ long *boolVal);
        
        END_INTERFACE
    } IUIAutomationBoolConditionVtbl;

    interface IUIAutomationBoolCondition
    {
        CONST_VTBL struct IUIAutomationBoolConditionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationBoolCondition_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationBoolCondition_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationBoolCondition_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 



#define IUIAutomationBoolCondition_get_BooleanValue(This,boolVal)	\
    ( (This)->lpVtbl -> get_BooleanValue(This,boolVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationBoolCondition_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationPropertyCondition_INTERFACE_DEFINED__
#define __IUIAutomationPropertyCondition_INTERFACE_DEFINED__

/* interface IUIAutomationPropertyCondition */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationPropertyCondition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("99EBF2CB-5578-4267-9AD4-AFD6EA77E94B")
    IUIAutomationPropertyCondition : public IUIAutomationCondition
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_propertyId( 
            /* [retval][out] */ int *propertyId) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_PropertyValue( 
            /* [retval][out] */ VARIANT *PropertyValue) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_PropertyConditionFlags( 
            /* [retval][out] */ PropertyConditionFlags *flags) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationPropertyConditionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationPropertyCondition * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationPropertyCondition * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationPropertyCondition * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_propertyId )( 
            IUIAutomationPropertyCondition * This,
            /* [retval][out] */ int *propertyId);
        
        /* [propget] */ HRESULT ( __stdcall *get_PropertyValue )( 
            IUIAutomationPropertyCondition * This,
            /* [retval][out] */ VARIANT *PropertyValue);
        
        /* [propget] */ HRESULT ( __stdcall *get_PropertyConditionFlags )( 
            IUIAutomationPropertyCondition * This,
            /* [retval][out] */ PropertyConditionFlags *flags);
        
        END_INTERFACE
    } IUIAutomationPropertyConditionVtbl;

    interface IUIAutomationPropertyCondition
    {
        CONST_VTBL struct IUIAutomationPropertyConditionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationPropertyCondition_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationPropertyCondition_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationPropertyCondition_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 



#define IUIAutomationPropertyCondition_get_propertyId(This,propertyId)	\
    ( (This)->lpVtbl -> get_propertyId(This,propertyId) ) 

#define IUIAutomationPropertyCondition_get_PropertyValue(This,PropertyValue)	\
    ( (This)->lpVtbl -> get_PropertyValue(This,PropertyValue) ) 

#define IUIAutomationPropertyCondition_get_PropertyConditionFlags(This,flags)	\
    ( (This)->lpVtbl -> get_PropertyConditionFlags(This,flags) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationPropertyCondition_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationAndCondition_INTERFACE_DEFINED__
#define __IUIAutomationAndCondition_INTERFACE_DEFINED__

/* interface IUIAutomationAndCondition */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationAndCondition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A7D0AF36-B912-45FE-9855-091DDC174AEC")
    IUIAutomationAndCondition : public IUIAutomationCondition
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_ChildCount( 
            /* [retval][out] */ int *ChildCount) = 0;
        
        virtual HRESULT __stdcall GetChildrenAsNativeArray( 
            /* [out] */ IUIAutomationCondition ***childArray,
            /* [out] */ int *childArrayCount) = 0;
        
        virtual HRESULT __stdcall GetChildren( 
            /* [retval][out] */ SAFEARRAY * *childArray) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationAndConditionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationAndCondition * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationAndCondition * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationAndCondition * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_ChildCount )( 
            IUIAutomationAndCondition * This,
            /* [retval][out] */ int *ChildCount);
        
        HRESULT ( __stdcall *GetChildrenAsNativeArray )( 
            IUIAutomationAndCondition * This,
            /* [out] */ IUIAutomationCondition ***childArray,
            /* [out] */ int *childArrayCount);
        
        HRESULT ( __stdcall *GetChildren )( 
            IUIAutomationAndCondition * This,
            /* [retval][out] */ SAFEARRAY * *childArray);
        
        END_INTERFACE
    } IUIAutomationAndConditionVtbl;

    interface IUIAutomationAndCondition
    {
        CONST_VTBL struct IUIAutomationAndConditionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationAndCondition_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationAndCondition_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationAndCondition_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 



#define IUIAutomationAndCondition_get_ChildCount(This,ChildCount)	\
    ( (This)->lpVtbl -> get_ChildCount(This,ChildCount) ) 

#define IUIAutomationAndCondition_GetChildrenAsNativeArray(This,childArray,childArrayCount)	\
    ( (This)->lpVtbl -> GetChildrenAsNativeArray(This,childArray,childArrayCount) ) 

#define IUIAutomationAndCondition_GetChildren(This,childArray)	\
    ( (This)->lpVtbl -> GetChildren(This,childArray) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationAndCondition_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationOrCondition_INTERFACE_DEFINED__
#define __IUIAutomationOrCondition_INTERFACE_DEFINED__

/* interface IUIAutomationOrCondition */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationOrCondition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8753F032-3DB1-47B5-A1FC-6E34A266C712")
    IUIAutomationOrCondition : public IUIAutomationCondition
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_ChildCount( 
            /* [retval][out] */ int *ChildCount) = 0;
        
        virtual HRESULT __stdcall GetChildrenAsNativeArray( 
            /* [out] */ IUIAutomationCondition ***childArray,
            /* [out] */ int *childArrayCount) = 0;
        
        virtual HRESULT __stdcall GetChildren( 
            /* [retval][out] */ SAFEARRAY * *childArray) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationOrConditionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationOrCondition * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationOrCondition * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationOrCondition * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_ChildCount )( 
            IUIAutomationOrCondition * This,
            /* [retval][out] */ int *ChildCount);
        
        HRESULT ( __stdcall *GetChildrenAsNativeArray )( 
            IUIAutomationOrCondition * This,
            /* [out] */ IUIAutomationCondition ***childArray,
            /* [out] */ int *childArrayCount);
        
        HRESULT ( __stdcall *GetChildren )( 
            IUIAutomationOrCondition * This,
            /* [retval][out] */ SAFEARRAY * *childArray);
        
        END_INTERFACE
    } IUIAutomationOrConditionVtbl;

    interface IUIAutomationOrCondition
    {
        CONST_VTBL struct IUIAutomationOrConditionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationOrCondition_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationOrCondition_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationOrCondition_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 



#define IUIAutomationOrCondition_get_ChildCount(This,ChildCount)	\
    ( (This)->lpVtbl -> get_ChildCount(This,ChildCount) ) 

#define IUIAutomationOrCondition_GetChildrenAsNativeArray(This,childArray,childArrayCount)	\
    ( (This)->lpVtbl -> GetChildrenAsNativeArray(This,childArray,childArrayCount) ) 

#define IUIAutomationOrCondition_GetChildren(This,childArray)	\
    ( (This)->lpVtbl -> GetChildren(This,childArray) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationOrCondition_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationNotCondition_INTERFACE_DEFINED__
#define __IUIAutomationNotCondition_INTERFACE_DEFINED__

/* interface IUIAutomationNotCondition */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationNotCondition;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F528B657-847B-498C-8896-D52B565407A1")
    IUIAutomationNotCondition : public IUIAutomationCondition
    {
    public:
        virtual HRESULT __stdcall GetChild( 
            /* [retval][out] */ IUIAutomationCondition **condition) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationNotConditionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationNotCondition * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationNotCondition * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationNotCondition * This);
        
        HRESULT ( __stdcall *GetChild )( 
            IUIAutomationNotCondition * This,
            /* [retval][out] */ IUIAutomationCondition **condition);
        
        END_INTERFACE
    } IUIAutomationNotConditionVtbl;

    interface IUIAutomationNotCondition
    {
        CONST_VTBL struct IUIAutomationNotConditionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationNotCondition_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationNotCondition_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationNotCondition_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 



#define IUIAutomationNotCondition_GetChild(This,condition)	\
    ( (This)->lpVtbl -> GetChild(This,condition) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationNotCondition_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationTreeWalker_INTERFACE_DEFINED__
#define __IUIAutomationTreeWalker_INTERFACE_DEFINED__

/* interface IUIAutomationTreeWalker */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationTreeWalker;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4042C624-389C-4AFC-A630-9DF854A541FC")
    IUIAutomationTreeWalker : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetParentElement( 
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **parent) = 0;
        
        virtual HRESULT __stdcall GetFirstChildElement( 
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **first) = 0;
        
        virtual HRESULT __stdcall GetLastChildElement( 
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **last) = 0;
        
        virtual HRESULT __stdcall GetNextSiblingElement( 
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **next) = 0;
        
        virtual HRESULT __stdcall GetPreviousSiblingElement( 
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **previous) = 0;
        
        virtual HRESULT __stdcall NormalizeElement( 
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **normalized) = 0;
        
        virtual HRESULT __stdcall GetParentElementBuildCache( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **parent) = 0;
        
        virtual HRESULT __stdcall GetFirstChildElementBuildCache( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **first) = 0;
        
        virtual HRESULT __stdcall GetLastChildElementBuildCache( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **last) = 0;
        
        virtual HRESULT __stdcall GetNextSiblingElementBuildCache( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **next) = 0;
        
        virtual HRESULT __stdcall GetPreviousSiblingElementBuildCache( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **previous) = 0;
        
        virtual HRESULT __stdcall NormalizeElementBuildCache( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **normalized) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_condition( 
            /* [retval][out] */ IUIAutomationCondition **condition) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationTreeWalkerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationTreeWalker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationTreeWalker * This);
        
        HRESULT ( __stdcall *GetParentElement )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **parent);
        
        HRESULT ( __stdcall *GetFirstChildElement )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **first);
        
        HRESULT ( __stdcall *GetLastChildElement )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **last);
        
        HRESULT ( __stdcall *GetNextSiblingElement )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **next);
        
        HRESULT ( __stdcall *GetPreviousSiblingElement )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **previous);
        
        HRESULT ( __stdcall *NormalizeElement )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [retval][out] */ IUIAutomationElement **normalized);
        
        HRESULT ( __stdcall *GetParentElementBuildCache )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **parent);
        
        HRESULT ( __stdcall *GetFirstChildElementBuildCache )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **first);
        
        HRESULT ( __stdcall *GetLastChildElementBuildCache )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **last);
        
        HRESULT ( __stdcall *GetNextSiblingElementBuildCache )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **next);
        
        HRESULT ( __stdcall *GetPreviousSiblingElementBuildCache )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **previous);
        
        HRESULT ( __stdcall *NormalizeElementBuildCache )( 
            IUIAutomationTreeWalker * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **normalized);
        
        /* [propget] */ HRESULT ( __stdcall *get_condition )( 
            IUIAutomationTreeWalker * This,
            /* [retval][out] */ IUIAutomationCondition **condition);
        
        END_INTERFACE
    } IUIAutomationTreeWalkerVtbl;

    interface IUIAutomationTreeWalker
    {
        CONST_VTBL struct IUIAutomationTreeWalkerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationTreeWalker_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationTreeWalker_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationTreeWalker_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationTreeWalker_GetParentElement(This,element,parent)	\
    ( (This)->lpVtbl -> GetParentElement(This,element,parent) ) 

#define IUIAutomationTreeWalker_GetFirstChildElement(This,element,first)	\
    ( (This)->lpVtbl -> GetFirstChildElement(This,element,first) ) 

#define IUIAutomationTreeWalker_GetLastChildElement(This,element,last)	\
    ( (This)->lpVtbl -> GetLastChildElement(This,element,last) ) 

#define IUIAutomationTreeWalker_GetNextSiblingElement(This,element,next)	\
    ( (This)->lpVtbl -> GetNextSiblingElement(This,element,next) ) 

#define IUIAutomationTreeWalker_GetPreviousSiblingElement(This,element,previous)	\
    ( (This)->lpVtbl -> GetPreviousSiblingElement(This,element,previous) ) 

#define IUIAutomationTreeWalker_NormalizeElement(This,element,normalized)	\
    ( (This)->lpVtbl -> NormalizeElement(This,element,normalized) ) 

#define IUIAutomationTreeWalker_GetParentElementBuildCache(This,element,cacheRequest,parent)	\
    ( (This)->lpVtbl -> GetParentElementBuildCache(This,element,cacheRequest,parent) ) 

#define IUIAutomationTreeWalker_GetFirstChildElementBuildCache(This,element,cacheRequest,first)	\
    ( (This)->lpVtbl -> GetFirstChildElementBuildCache(This,element,cacheRequest,first) ) 

#define IUIAutomationTreeWalker_GetLastChildElementBuildCache(This,element,cacheRequest,last)	\
    ( (This)->lpVtbl -> GetLastChildElementBuildCache(This,element,cacheRequest,last) ) 

#define IUIAutomationTreeWalker_GetNextSiblingElementBuildCache(This,element,cacheRequest,next)	\
    ( (This)->lpVtbl -> GetNextSiblingElementBuildCache(This,element,cacheRequest,next) ) 

#define IUIAutomationTreeWalker_GetPreviousSiblingElementBuildCache(This,element,cacheRequest,previous)	\
    ( (This)->lpVtbl -> GetPreviousSiblingElementBuildCache(This,element,cacheRequest,previous) ) 

#define IUIAutomationTreeWalker_NormalizeElementBuildCache(This,element,cacheRequest,normalized)	\
    ( (This)->lpVtbl -> NormalizeElementBuildCache(This,element,cacheRequest,normalized) ) 

#define IUIAutomationTreeWalker_get_condition(This,condition)	\
    ( (This)->lpVtbl -> get_condition(This,condition) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationTreeWalker_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationEventHandler_INTERFACE_DEFINED__
#define __IUIAutomationEventHandler_INTERFACE_DEFINED__

/* interface IUIAutomationEventHandler */
/* [object][oleautomation][uuid] */ 


EXTERN_C const IID IID_IUIAutomationEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("146C3C17-F12E-4E22-8C27-F894B9B79C69")
    IUIAutomationEventHandler : public IUnknown
    {
    public:
        virtual HRESULT __stdcall HandleAutomationEvent( 
            /* [in] */ IUIAutomationElement *sender,
            /* [in] */ int eventId) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationEventHandler * This);
        
        HRESULT ( __stdcall *HandleAutomationEvent )( 
            IUIAutomationEventHandler * This,
            /* [in] */ IUIAutomationElement *sender,
            /* [in] */ int eventId);
        
        END_INTERFACE
    } IUIAutomationEventHandlerVtbl;

    interface IUIAutomationEventHandler
    {
        CONST_VTBL struct IUIAutomationEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationEventHandler_HandleAutomationEvent(This,sender,eventId)	\
    ( (This)->lpVtbl -> HandleAutomationEvent(This,sender,eventId) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationEventHandler_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationPropertyChangedEventHandler_INTERFACE_DEFINED__
#define __IUIAutomationPropertyChangedEventHandler_INTERFACE_DEFINED__

/* interface IUIAutomationPropertyChangedEventHandler */
/* [object][oleautomation][uuid] */ 


EXTERN_C const IID IID_IUIAutomationPropertyChangedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("40CD37D4-C756-4B0C-8C6F-BDDFEEB13B50")
    IUIAutomationPropertyChangedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT __stdcall HandlePropertyChangedEvent( 
            /* [in] */ IUIAutomationElement *sender,
            /* [in] */ int propertyId,
            /* [in] */ VARIANT newValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationPropertyChangedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationPropertyChangedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationPropertyChangedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationPropertyChangedEventHandler * This);
        
        HRESULT ( __stdcall *HandlePropertyChangedEvent )( 
            IUIAutomationPropertyChangedEventHandler * This,
            /* [in] */ IUIAutomationElement *sender,
            /* [in] */ int propertyId,
            /* [in] */ VARIANT newValue);
        
        END_INTERFACE
    } IUIAutomationPropertyChangedEventHandlerVtbl;

    interface IUIAutomationPropertyChangedEventHandler
    {
        CONST_VTBL struct IUIAutomationPropertyChangedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationPropertyChangedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationPropertyChangedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationPropertyChangedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationPropertyChangedEventHandler_HandlePropertyChangedEvent(This,sender,propertyId,newValue)	\
    ( (This)->lpVtbl -> HandlePropertyChangedEvent(This,sender,propertyId,newValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationPropertyChangedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationStructureChangedEventHandler_INTERFACE_DEFINED__
#define __IUIAutomationStructureChangedEventHandler_INTERFACE_DEFINED__

/* interface IUIAutomationStructureChangedEventHandler */
/* [object][oleautomation][uuid] */ 


EXTERN_C const IID IID_IUIAutomationStructureChangedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E81D1B4E-11C5-42F8-9754-E7036C79F054")
    IUIAutomationStructureChangedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT __stdcall HandleStructureChangedEvent( 
            /* [in] */ IUIAutomationElement *sender,
            /* [in] */ StructureChangeType changeType,
            /* [in] */ SAFEARRAY * runtimeId) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationStructureChangedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationStructureChangedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationStructureChangedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationStructureChangedEventHandler * This);
        
        HRESULT ( __stdcall *HandleStructureChangedEvent )( 
            IUIAutomationStructureChangedEventHandler * This,
            /* [in] */ IUIAutomationElement *sender,
            /* [in] */ StructureChangeType changeType,
            /* [in] */ SAFEARRAY * runtimeId);
        
        END_INTERFACE
    } IUIAutomationStructureChangedEventHandlerVtbl;

    interface IUIAutomationStructureChangedEventHandler
    {
        CONST_VTBL struct IUIAutomationStructureChangedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationStructureChangedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationStructureChangedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationStructureChangedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationStructureChangedEventHandler_HandleStructureChangedEvent(This,sender,changeType,runtimeId)	\
    ( (This)->lpVtbl -> HandleStructureChangedEvent(This,sender,changeType,runtimeId) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationStructureChangedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationFocusChangedEventHandler_INTERFACE_DEFINED__
#define __IUIAutomationFocusChangedEventHandler_INTERFACE_DEFINED__

/* interface IUIAutomationFocusChangedEventHandler */
/* [object][oleautomation][uuid] */ 


EXTERN_C const IID IID_IUIAutomationFocusChangedEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C270F6B5-5C69-4290-9745-7A7F97169468")
    IUIAutomationFocusChangedEventHandler : public IUnknown
    {
    public:
        virtual HRESULT __stdcall HandleFocusChangedEvent( 
            /* [in] */ IUIAutomationElement *sender) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationFocusChangedEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationFocusChangedEventHandler * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationFocusChangedEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationFocusChangedEventHandler * This);
        
        HRESULT ( __stdcall *HandleFocusChangedEvent )( 
            IUIAutomationFocusChangedEventHandler * This,
            /* [in] */ IUIAutomationElement *sender);
        
        END_INTERFACE
    } IUIAutomationFocusChangedEventHandlerVtbl;

    interface IUIAutomationFocusChangedEventHandler
    {
        CONST_VTBL struct IUIAutomationFocusChangedEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationFocusChangedEventHandler_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationFocusChangedEventHandler_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationFocusChangedEventHandler_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationFocusChangedEventHandler_HandleFocusChangedEvent(This,sender)	\
    ( (This)->lpVtbl -> HandleFocusChangedEvent(This,sender) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationFocusChangedEventHandler_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationInvokePattern_INTERFACE_DEFINED__
#define __IUIAutomationInvokePattern_INTERFACE_DEFINED__

/* interface IUIAutomationInvokePattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationInvokePattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FB377FBE-8EA6-46D5-9C73-6499642D3059")
    IUIAutomationInvokePattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Invoke( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationInvokePatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationInvokePattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationInvokePattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationInvokePattern * This);
        
        HRESULT ( __stdcall *Invoke )( 
            IUIAutomationInvokePattern * This);
        
        END_INTERFACE
    } IUIAutomationInvokePatternVtbl;

    interface IUIAutomationInvokePattern
    {
        CONST_VTBL struct IUIAutomationInvokePatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationInvokePattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationInvokePattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationInvokePattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationInvokePattern_Invoke(This)	\
    ( (This)->lpVtbl -> Invoke(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationInvokePattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationDockPattern_INTERFACE_DEFINED__
#define __IUIAutomationDockPattern_INTERFACE_DEFINED__

/* interface IUIAutomationDockPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationDockPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FDE5EF97-1464-48F6-90BF-43D0948E86EC")
    IUIAutomationDockPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall SetDockPosition( 
            /* [in] */ DockPosition dockPos) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentDockPosition( 
            /* [retval][out] */ DockPosition *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedDockPosition( 
            /* [retval][out] */ DockPosition *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationDockPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationDockPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationDockPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationDockPattern * This);
        
        HRESULT ( __stdcall *SetDockPosition )( 
            IUIAutomationDockPattern * This,
            /* [in] */ DockPosition dockPos);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentDockPosition )( 
            IUIAutomationDockPattern * This,
            /* [retval][out] */ DockPosition *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedDockPosition )( 
            IUIAutomationDockPattern * This,
            /* [retval][out] */ DockPosition *retVal);
        
        END_INTERFACE
    } IUIAutomationDockPatternVtbl;

    interface IUIAutomationDockPattern
    {
        CONST_VTBL struct IUIAutomationDockPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationDockPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationDockPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationDockPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationDockPattern_SetDockPosition(This,dockPos)	\
    ( (This)->lpVtbl -> SetDockPosition(This,dockPos) ) 

#define IUIAutomationDockPattern_get_CurrentDockPosition(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentDockPosition(This,retVal) ) 

#define IUIAutomationDockPattern_get_CachedDockPosition(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedDockPosition(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationDockPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationExpandCollapsePattern_INTERFACE_DEFINED__
#define __IUIAutomationExpandCollapsePattern_INTERFACE_DEFINED__

/* interface IUIAutomationExpandCollapsePattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationExpandCollapsePattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("619BE086-1F4E-4EE4-BAFA-210128738730")
    IUIAutomationExpandCollapsePattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Expand( void) = 0;
        
        virtual HRESULT __stdcall Collapse( void) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentExpandCollapseState( 
            /* [retval][out] */ ExpandCollapseState *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedExpandCollapseState( 
            /* [retval][out] */ ExpandCollapseState *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationExpandCollapsePatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationExpandCollapsePattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationExpandCollapsePattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationExpandCollapsePattern * This);
        
        HRESULT ( __stdcall *Expand )( 
            IUIAutomationExpandCollapsePattern * This);
        
        HRESULT ( __stdcall *Collapse )( 
            IUIAutomationExpandCollapsePattern * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentExpandCollapseState )( 
            IUIAutomationExpandCollapsePattern * This,
            /* [retval][out] */ ExpandCollapseState *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedExpandCollapseState )( 
            IUIAutomationExpandCollapsePattern * This,
            /* [retval][out] */ ExpandCollapseState *retVal);
        
        END_INTERFACE
    } IUIAutomationExpandCollapsePatternVtbl;

    interface IUIAutomationExpandCollapsePattern
    {
        CONST_VTBL struct IUIAutomationExpandCollapsePatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationExpandCollapsePattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationExpandCollapsePattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationExpandCollapsePattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationExpandCollapsePattern_Expand(This)	\
    ( (This)->lpVtbl -> Expand(This) ) 

#define IUIAutomationExpandCollapsePattern_Collapse(This)	\
    ( (This)->lpVtbl -> Collapse(This) ) 

#define IUIAutomationExpandCollapsePattern_get_CurrentExpandCollapseState(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentExpandCollapseState(This,retVal) ) 

#define IUIAutomationExpandCollapsePattern_get_CachedExpandCollapseState(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedExpandCollapseState(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationExpandCollapsePattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationGridPattern_INTERFACE_DEFINED__
#define __IUIAutomationGridPattern_INTERFACE_DEFINED__

/* interface IUIAutomationGridPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationGridPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("414C3CDC-856B-4F5B-8538-3131C6302550")
    IUIAutomationGridPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetItem( 
            /* [in] */ int row,
            /* [in] */ int column,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentRowCount( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentColumnCount( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedRowCount( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedColumnCount( 
            /* [retval][out] */ int *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationGridPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationGridPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationGridPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationGridPattern * This);
        
        HRESULT ( __stdcall *GetItem )( 
            IUIAutomationGridPattern * This,
            /* [in] */ int row,
            /* [in] */ int column,
            /* [retval][out] */ IUIAutomationElement **element);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentRowCount )( 
            IUIAutomationGridPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentColumnCount )( 
            IUIAutomationGridPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedRowCount )( 
            IUIAutomationGridPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedColumnCount )( 
            IUIAutomationGridPattern * This,
            /* [retval][out] */ int *retVal);
        
        END_INTERFACE
    } IUIAutomationGridPatternVtbl;

    interface IUIAutomationGridPattern
    {
        CONST_VTBL struct IUIAutomationGridPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationGridPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationGridPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationGridPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationGridPattern_GetItem(This,row,column,element)	\
    ( (This)->lpVtbl -> GetItem(This,row,column,element) ) 

#define IUIAutomationGridPattern_get_CurrentRowCount(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentRowCount(This,retVal) ) 

#define IUIAutomationGridPattern_get_CurrentColumnCount(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentColumnCount(This,retVal) ) 

#define IUIAutomationGridPattern_get_CachedRowCount(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedRowCount(This,retVal) ) 

#define IUIAutomationGridPattern_get_CachedColumnCount(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedColumnCount(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationGridPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationGridItemPattern_INTERFACE_DEFINED__
#define __IUIAutomationGridItemPattern_INTERFACE_DEFINED__

/* interface IUIAutomationGridItemPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationGridItemPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("78F8EF57-66C3-4E09-BD7C-E79B2004894D")
    IUIAutomationGridItemPattern : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_CurrentContainingGrid( 
            /* [retval][out] */ IUIAutomationElement **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentRow( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentColumn( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentRowSpan( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentColumnSpan( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedContainingGrid( 
            /* [retval][out] */ IUIAutomationElement **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedRow( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedColumn( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedRowSpan( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedColumnSpan( 
            /* [retval][out] */ int *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationGridItemPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationGridItemPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationGridItemPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationGridItemPattern * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentContainingGrid )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ IUIAutomationElement **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentRow )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentColumn )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentRowSpan )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentColumnSpan )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedContainingGrid )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ IUIAutomationElement **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedRow )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedColumn )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedRowSpan )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ int *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedColumnSpan )( 
            IUIAutomationGridItemPattern * This,
            /* [retval][out] */ int *retVal);
        
        END_INTERFACE
    } IUIAutomationGridItemPatternVtbl;

    interface IUIAutomationGridItemPattern
    {
        CONST_VTBL struct IUIAutomationGridItemPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationGridItemPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationGridItemPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationGridItemPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationGridItemPattern_get_CurrentContainingGrid(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentContainingGrid(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CurrentRow(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentRow(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CurrentColumn(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentColumn(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CurrentRowSpan(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentRowSpan(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CurrentColumnSpan(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentColumnSpan(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CachedContainingGrid(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedContainingGrid(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CachedRow(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedRow(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CachedColumn(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedColumn(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CachedRowSpan(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedRowSpan(This,retVal) ) 

#define IUIAutomationGridItemPattern_get_CachedColumnSpan(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedColumnSpan(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationGridItemPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationMultipleViewPattern_INTERFACE_DEFINED__
#define __IUIAutomationMultipleViewPattern_INTERFACE_DEFINED__

/* interface IUIAutomationMultipleViewPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationMultipleViewPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8D253C91-1DC5-4BB5-B18F-ADE16FA495E8")
    IUIAutomationMultipleViewPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetViewName( 
            /* [in] */ int view,
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual HRESULT __stdcall SetCurrentView( 
            /* [in] */ int view) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentCurrentView( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual HRESULT __stdcall GetCurrentSupportedViews( 
            /* [retval][out] */ SAFEARRAY * *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedCurrentView( 
            /* [retval][out] */ int *retVal) = 0;
        
        virtual HRESULT __stdcall GetCachedSupportedViews( 
            /* [retval][out] */ SAFEARRAY * *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationMultipleViewPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationMultipleViewPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationMultipleViewPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationMultipleViewPattern * This);
        
        HRESULT ( __stdcall *GetViewName )( 
            IUIAutomationMultipleViewPattern * This,
            /* [in] */ int view,
            /* [retval][out] */ BSTR *name);
        
        HRESULT ( __stdcall *SetCurrentView )( 
            IUIAutomationMultipleViewPattern * This,
            /* [in] */ int view);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentCurrentView )( 
            IUIAutomationMultipleViewPattern * This,
            /* [retval][out] */ int *retVal);
        
        HRESULT ( __stdcall *GetCurrentSupportedViews )( 
            IUIAutomationMultipleViewPattern * This,
            /* [retval][out] */ SAFEARRAY * *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedCurrentView )( 
            IUIAutomationMultipleViewPattern * This,
            /* [retval][out] */ int *retVal);
        
        HRESULT ( __stdcall *GetCachedSupportedViews )( 
            IUIAutomationMultipleViewPattern * This,
            /* [retval][out] */ SAFEARRAY * *retVal);
        
        END_INTERFACE
    } IUIAutomationMultipleViewPatternVtbl;

    interface IUIAutomationMultipleViewPattern
    {
        CONST_VTBL struct IUIAutomationMultipleViewPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationMultipleViewPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationMultipleViewPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationMultipleViewPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationMultipleViewPattern_GetViewName(This,view,name)	\
    ( (This)->lpVtbl -> GetViewName(This,view,name) ) 

#define IUIAutomationMultipleViewPattern_SetCurrentView(This,view)	\
    ( (This)->lpVtbl -> SetCurrentView(This,view) ) 

#define IUIAutomationMultipleViewPattern_get_CurrentCurrentView(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentCurrentView(This,retVal) ) 

#define IUIAutomationMultipleViewPattern_GetCurrentSupportedViews(This,retVal)	\
    ( (This)->lpVtbl -> GetCurrentSupportedViews(This,retVal) ) 

#define IUIAutomationMultipleViewPattern_get_CachedCurrentView(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedCurrentView(This,retVal) ) 

#define IUIAutomationMultipleViewPattern_GetCachedSupportedViews(This,retVal)	\
    ( (This)->lpVtbl -> GetCachedSupportedViews(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationMultipleViewPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationRangeValuePattern_INTERFACE_DEFINED__
#define __IUIAutomationRangeValuePattern_INTERFACE_DEFINED__

/* interface IUIAutomationRangeValuePattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationRangeValuePattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("59213F4F-7346-49E5-B120-80555987A148")
    IUIAutomationRangeValuePattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall SetValue( 
            /* [in] */ double val) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentValue( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsReadOnly( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentMaximum( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentMinimum( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentLargeChange( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentSmallChange( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedValue( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsReadOnly( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedMaximum( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedMinimum( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedLargeChange( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedSmallChange( 
            /* [retval][out] */ double *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationRangeValuePatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationRangeValuePattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationRangeValuePattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationRangeValuePattern * This);
        
        HRESULT ( __stdcall *SetValue )( 
            IUIAutomationRangeValuePattern * This,
            /* [in] */ double val);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentValue )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsReadOnly )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentMaximum )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentMinimum )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentLargeChange )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentSmallChange )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedValue )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsReadOnly )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedMaximum )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedMinimum )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedLargeChange )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedSmallChange )( 
            IUIAutomationRangeValuePattern * This,
            /* [retval][out] */ double *retVal);
        
        END_INTERFACE
    } IUIAutomationRangeValuePatternVtbl;

    interface IUIAutomationRangeValuePattern
    {
        CONST_VTBL struct IUIAutomationRangeValuePatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationRangeValuePattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationRangeValuePattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationRangeValuePattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationRangeValuePattern_SetValue(This,val)	\
    ( (This)->lpVtbl -> SetValue(This,val) ) 

#define IUIAutomationRangeValuePattern_get_CurrentValue(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentValue(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CurrentIsReadOnly(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsReadOnly(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CurrentMaximum(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentMaximum(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CurrentMinimum(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentMinimum(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CurrentLargeChange(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentLargeChange(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CurrentSmallChange(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentSmallChange(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CachedValue(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedValue(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CachedIsReadOnly(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsReadOnly(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CachedMaximum(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedMaximum(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CachedMinimum(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedMinimum(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CachedLargeChange(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedLargeChange(This,retVal) ) 

#define IUIAutomationRangeValuePattern_get_CachedSmallChange(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedSmallChange(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationRangeValuePattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationScrollPattern_INTERFACE_DEFINED__
#define __IUIAutomationScrollPattern_INTERFACE_DEFINED__

/* interface IUIAutomationScrollPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationScrollPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88F4D42A-E881-459D-A77C-73BBBB7E02DC")
    IUIAutomationScrollPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Scroll( 
            /* [in] */ ScrollAmount horizontalAmount,
            /* [in] */ ScrollAmount verticalAmount) = 0;
        
        virtual HRESULT __stdcall SetScrollPercent( 
            /* [in] */ double horizontalPercent,
            /* [in] */ double verticalPercent) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentHorizontalScrollPercent( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentVerticalScrollPercent( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentHorizontalViewSize( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentVerticalViewSize( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentHorizontallyScrollable( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentVerticallyScrollable( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedHorizontalScrollPercent( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedVerticalScrollPercent( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedHorizontalViewSize( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedVerticalViewSize( 
            /* [retval][out] */ double *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedHorizontallyScrollable( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedVerticallyScrollable( 
            /* [retval][out] */ long *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationScrollPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationScrollPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationScrollPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationScrollPattern * This);
        
        HRESULT ( __stdcall *Scroll )( 
            IUIAutomationScrollPattern * This,
            /* [in] */ ScrollAmount horizontalAmount,
            /* [in] */ ScrollAmount verticalAmount);
        
        HRESULT ( __stdcall *SetScrollPercent )( 
            IUIAutomationScrollPattern * This,
            /* [in] */ double horizontalPercent,
            /* [in] */ double verticalPercent);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentHorizontalScrollPercent )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentVerticalScrollPercent )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentHorizontalViewSize )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentVerticalViewSize )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentHorizontallyScrollable )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentVerticallyScrollable )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedHorizontalScrollPercent )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedVerticalScrollPercent )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedHorizontalViewSize )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedVerticalViewSize )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ double *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedHorizontallyScrollable )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedVerticallyScrollable )( 
            IUIAutomationScrollPattern * This,
            /* [retval][out] */ long *retVal);
        
        END_INTERFACE
    } IUIAutomationScrollPatternVtbl;

    interface IUIAutomationScrollPattern
    {
        CONST_VTBL struct IUIAutomationScrollPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationScrollPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationScrollPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationScrollPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationScrollPattern_Scroll(This,horizontalAmount,verticalAmount)	\
    ( (This)->lpVtbl -> Scroll(This,horizontalAmount,verticalAmount) ) 

#define IUIAutomationScrollPattern_SetScrollPercent(This,horizontalPercent,verticalPercent)	\
    ( (This)->lpVtbl -> SetScrollPercent(This,horizontalPercent,verticalPercent) ) 

#define IUIAutomationScrollPattern_get_CurrentHorizontalScrollPercent(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentHorizontalScrollPercent(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CurrentVerticalScrollPercent(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentVerticalScrollPercent(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CurrentHorizontalViewSize(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentHorizontalViewSize(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CurrentVerticalViewSize(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentVerticalViewSize(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CurrentHorizontallyScrollable(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentHorizontallyScrollable(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CurrentVerticallyScrollable(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentVerticallyScrollable(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CachedHorizontalScrollPercent(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedHorizontalScrollPercent(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CachedVerticalScrollPercent(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedVerticalScrollPercent(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CachedHorizontalViewSize(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedHorizontalViewSize(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CachedVerticalViewSize(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedVerticalViewSize(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CachedHorizontallyScrollable(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedHorizontallyScrollable(This,retVal) ) 

#define IUIAutomationScrollPattern_get_CachedVerticallyScrollable(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedVerticallyScrollable(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationScrollPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationScrollItemPattern_INTERFACE_DEFINED__
#define __IUIAutomationScrollItemPattern_INTERFACE_DEFINED__

/* interface IUIAutomationScrollItemPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationScrollItemPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B488300F-D015-4F19-9C29-BB595E3645EF")
    IUIAutomationScrollItemPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall ScrollIntoView( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationScrollItemPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationScrollItemPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationScrollItemPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationScrollItemPattern * This);
        
        HRESULT ( __stdcall *ScrollIntoView )( 
            IUIAutomationScrollItemPattern * This);
        
        END_INTERFACE
    } IUIAutomationScrollItemPatternVtbl;

    interface IUIAutomationScrollItemPattern
    {
        CONST_VTBL struct IUIAutomationScrollItemPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationScrollItemPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationScrollItemPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationScrollItemPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationScrollItemPattern_ScrollIntoView(This)	\
    ( (This)->lpVtbl -> ScrollIntoView(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationScrollItemPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationSelectionPattern_INTERFACE_DEFINED__
#define __IUIAutomationSelectionPattern_INTERFACE_DEFINED__

/* interface IUIAutomationSelectionPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationSelectionPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5ED5202E-B2AC-47A6-B638-4B0BF140D78E")
    IUIAutomationSelectionPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetCurrentSelection( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentCanSelectMultiple( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsSelectionRequired( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual HRESULT __stdcall GetCachedSelection( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedCanSelectMultiple( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsSelectionRequired( 
            /* [retval][out] */ long *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationSelectionPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationSelectionPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationSelectionPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationSelectionPattern * This);
        
        HRESULT ( __stdcall *GetCurrentSelection )( 
            IUIAutomationSelectionPattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentCanSelectMultiple )( 
            IUIAutomationSelectionPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsSelectionRequired )( 
            IUIAutomationSelectionPattern * This,
            /* [retval][out] */ long *retVal);
        
        HRESULT ( __stdcall *GetCachedSelection )( 
            IUIAutomationSelectionPattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedCanSelectMultiple )( 
            IUIAutomationSelectionPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsSelectionRequired )( 
            IUIAutomationSelectionPattern * This,
            /* [retval][out] */ long *retVal);
        
        END_INTERFACE
    } IUIAutomationSelectionPatternVtbl;

    interface IUIAutomationSelectionPattern
    {
        CONST_VTBL struct IUIAutomationSelectionPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationSelectionPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationSelectionPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationSelectionPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationSelectionPattern_GetCurrentSelection(This,retVal)	\
    ( (This)->lpVtbl -> GetCurrentSelection(This,retVal) ) 

#define IUIAutomationSelectionPattern_get_CurrentCanSelectMultiple(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentCanSelectMultiple(This,retVal) ) 

#define IUIAutomationSelectionPattern_get_CurrentIsSelectionRequired(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsSelectionRequired(This,retVal) ) 

#define IUIAutomationSelectionPattern_GetCachedSelection(This,retVal)	\
    ( (This)->lpVtbl -> GetCachedSelection(This,retVal) ) 

#define IUIAutomationSelectionPattern_get_CachedCanSelectMultiple(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedCanSelectMultiple(This,retVal) ) 

#define IUIAutomationSelectionPattern_get_CachedIsSelectionRequired(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsSelectionRequired(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationSelectionPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationSelectionItemPattern_INTERFACE_DEFINED__
#define __IUIAutomationSelectionItemPattern_INTERFACE_DEFINED__

/* interface IUIAutomationSelectionItemPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationSelectionItemPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A8EFA66A-0FDA-421A-9194-38021F3578EA")
    IUIAutomationSelectionItemPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Select( void) = 0;
        
        virtual HRESULT __stdcall AddToSelection( void) = 0;
        
        virtual HRESULT __stdcall RemoveFromSelection( void) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsSelected( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentSelectionContainer( 
            /* [retval][out] */ IUIAutomationElement **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsSelected( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedSelectionContainer( 
            /* [retval][out] */ IUIAutomationElement **retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationSelectionItemPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationSelectionItemPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationSelectionItemPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationSelectionItemPattern * This);
        
        HRESULT ( __stdcall *Select )( 
            IUIAutomationSelectionItemPattern * This);
        
        HRESULT ( __stdcall *AddToSelection )( 
            IUIAutomationSelectionItemPattern * This);
        
        HRESULT ( __stdcall *RemoveFromSelection )( 
            IUIAutomationSelectionItemPattern * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsSelected )( 
            IUIAutomationSelectionItemPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentSelectionContainer )( 
            IUIAutomationSelectionItemPattern * This,
            /* [retval][out] */ IUIAutomationElement **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsSelected )( 
            IUIAutomationSelectionItemPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedSelectionContainer )( 
            IUIAutomationSelectionItemPattern * This,
            /* [retval][out] */ IUIAutomationElement **retVal);
        
        END_INTERFACE
    } IUIAutomationSelectionItemPatternVtbl;

    interface IUIAutomationSelectionItemPattern
    {
        CONST_VTBL struct IUIAutomationSelectionItemPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationSelectionItemPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationSelectionItemPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationSelectionItemPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationSelectionItemPattern_Select(This)	\
    ( (This)->lpVtbl -> Select(This) ) 

#define IUIAutomationSelectionItemPattern_AddToSelection(This)	\
    ( (This)->lpVtbl -> AddToSelection(This) ) 

#define IUIAutomationSelectionItemPattern_RemoveFromSelection(This)	\
    ( (This)->lpVtbl -> RemoveFromSelection(This) ) 

#define IUIAutomationSelectionItemPattern_get_CurrentIsSelected(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsSelected(This,retVal) ) 

#define IUIAutomationSelectionItemPattern_get_CurrentSelectionContainer(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentSelectionContainer(This,retVal) ) 

#define IUIAutomationSelectionItemPattern_get_CachedIsSelected(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsSelected(This,retVal) ) 

#define IUIAutomationSelectionItemPattern_get_CachedSelectionContainer(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedSelectionContainer(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationSelectionItemPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationSynchronizedInputPattern_INTERFACE_DEFINED__
#define __IUIAutomationSynchronizedInputPattern_INTERFACE_DEFINED__

/* interface IUIAutomationSynchronizedInputPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationSynchronizedInputPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2233BE0B-AFB7-448B-9FDA-3B378AA5EAE1")
    IUIAutomationSynchronizedInputPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall StartListening( 
            /* [in] */ SynchronizedInputType inputType) = 0;
        
        virtual HRESULT __stdcall Cancel( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationSynchronizedInputPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationSynchronizedInputPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationSynchronizedInputPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationSynchronizedInputPattern * This);
        
        HRESULT ( __stdcall *StartListening )( 
            IUIAutomationSynchronizedInputPattern * This,
            /* [in] */ SynchronizedInputType inputType);
        
        HRESULT ( __stdcall *Cancel )( 
            IUIAutomationSynchronizedInputPattern * This);
        
        END_INTERFACE
    } IUIAutomationSynchronizedInputPatternVtbl;

    interface IUIAutomationSynchronizedInputPattern
    {
        CONST_VTBL struct IUIAutomationSynchronizedInputPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationSynchronizedInputPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationSynchronizedInputPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationSynchronizedInputPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationSynchronizedInputPattern_StartListening(This,inputType)	\
    ( (This)->lpVtbl -> StartListening(This,inputType) ) 

#define IUIAutomationSynchronizedInputPattern_Cancel(This)	\
    ( (This)->lpVtbl -> Cancel(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationSynchronizedInputPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationTablePattern_INTERFACE_DEFINED__
#define __IUIAutomationTablePattern_INTERFACE_DEFINED__

/* interface IUIAutomationTablePattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationTablePattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("620E691C-EA96-4710-A850-754B24CE2417")
    IUIAutomationTablePattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetCurrentRowHeaders( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual HRESULT __stdcall GetCurrentColumnHeaders( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentRowOrColumnMajor( 
            /* [retval][out] */ RowOrColumnMajor *retVal) = 0;
        
        virtual HRESULT __stdcall GetCachedRowHeaders( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual HRESULT __stdcall GetCachedColumnHeaders( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedRowOrColumnMajor( 
            /* [retval][out] */ RowOrColumnMajor *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationTablePatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationTablePattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationTablePattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationTablePattern * This);
        
        HRESULT ( __stdcall *GetCurrentRowHeaders )( 
            IUIAutomationTablePattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        HRESULT ( __stdcall *GetCurrentColumnHeaders )( 
            IUIAutomationTablePattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentRowOrColumnMajor )( 
            IUIAutomationTablePattern * This,
            /* [retval][out] */ RowOrColumnMajor *retVal);
        
        HRESULT ( __stdcall *GetCachedRowHeaders )( 
            IUIAutomationTablePattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        HRESULT ( __stdcall *GetCachedColumnHeaders )( 
            IUIAutomationTablePattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedRowOrColumnMajor )( 
            IUIAutomationTablePattern * This,
            /* [retval][out] */ RowOrColumnMajor *retVal);
        
        END_INTERFACE
    } IUIAutomationTablePatternVtbl;

    interface IUIAutomationTablePattern
    {
        CONST_VTBL struct IUIAutomationTablePatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationTablePattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationTablePattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationTablePattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationTablePattern_GetCurrentRowHeaders(This,retVal)	\
    ( (This)->lpVtbl -> GetCurrentRowHeaders(This,retVal) ) 

#define IUIAutomationTablePattern_GetCurrentColumnHeaders(This,retVal)	\
    ( (This)->lpVtbl -> GetCurrentColumnHeaders(This,retVal) ) 

#define IUIAutomationTablePattern_get_CurrentRowOrColumnMajor(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentRowOrColumnMajor(This,retVal) ) 

#define IUIAutomationTablePattern_GetCachedRowHeaders(This,retVal)	\
    ( (This)->lpVtbl -> GetCachedRowHeaders(This,retVal) ) 

#define IUIAutomationTablePattern_GetCachedColumnHeaders(This,retVal)	\
    ( (This)->lpVtbl -> GetCachedColumnHeaders(This,retVal) ) 

#define IUIAutomationTablePattern_get_CachedRowOrColumnMajor(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedRowOrColumnMajor(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationTablePattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationTableItemPattern_INTERFACE_DEFINED__
#define __IUIAutomationTableItemPattern_INTERFACE_DEFINED__

/* interface IUIAutomationTableItemPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationTableItemPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0B964EB3-EF2E-4464-9C79-61D61737A27E")
    IUIAutomationTableItemPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetCurrentRowHeaderItems( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual HRESULT __stdcall GetCurrentColumnHeaderItems( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual HRESULT __stdcall GetCachedRowHeaderItems( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
        virtual HRESULT __stdcall GetCachedColumnHeaderItems( 
            /* [retval][out] */ IUIAutomationElementArray **retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationTableItemPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationTableItemPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationTableItemPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationTableItemPattern * This);
        
        HRESULT ( __stdcall *GetCurrentRowHeaderItems )( 
            IUIAutomationTableItemPattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        HRESULT ( __stdcall *GetCurrentColumnHeaderItems )( 
            IUIAutomationTableItemPattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        HRESULT ( __stdcall *GetCachedRowHeaderItems )( 
            IUIAutomationTableItemPattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        HRESULT ( __stdcall *GetCachedColumnHeaderItems )( 
            IUIAutomationTableItemPattern * This,
            /* [retval][out] */ IUIAutomationElementArray **retVal);
        
        END_INTERFACE
    } IUIAutomationTableItemPatternVtbl;

    interface IUIAutomationTableItemPattern
    {
        CONST_VTBL struct IUIAutomationTableItemPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationTableItemPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationTableItemPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationTableItemPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationTableItemPattern_GetCurrentRowHeaderItems(This,retVal)	\
    ( (This)->lpVtbl -> GetCurrentRowHeaderItems(This,retVal) ) 

#define IUIAutomationTableItemPattern_GetCurrentColumnHeaderItems(This,retVal)	\
    ( (This)->lpVtbl -> GetCurrentColumnHeaderItems(This,retVal) ) 

#define IUIAutomationTableItemPattern_GetCachedRowHeaderItems(This,retVal)	\
    ( (This)->lpVtbl -> GetCachedRowHeaderItems(This,retVal) ) 

#define IUIAutomationTableItemPattern_GetCachedColumnHeaderItems(This,retVal)	\
    ( (This)->lpVtbl -> GetCachedColumnHeaderItems(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationTableItemPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationTogglePattern_INTERFACE_DEFINED__
#define __IUIAutomationTogglePattern_INTERFACE_DEFINED__

/* interface IUIAutomationTogglePattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationTogglePattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("94CF8058-9B8D-4AB9-8BFD-4CD0A33C8C70")
    IUIAutomationTogglePattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Toggle( void) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentToggleState( 
            /* [retval][out] */ ToggleState *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedToggleState( 
            /* [retval][out] */ ToggleState *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationTogglePatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationTogglePattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationTogglePattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationTogglePattern * This);
        
        HRESULT ( __stdcall *Toggle )( 
            IUIAutomationTogglePattern * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentToggleState )( 
            IUIAutomationTogglePattern * This,
            /* [retval][out] */ ToggleState *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedToggleState )( 
            IUIAutomationTogglePattern * This,
            /* [retval][out] */ ToggleState *retVal);
        
        END_INTERFACE
    } IUIAutomationTogglePatternVtbl;

    interface IUIAutomationTogglePattern
    {
        CONST_VTBL struct IUIAutomationTogglePatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationTogglePattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationTogglePattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationTogglePattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationTogglePattern_Toggle(This)	\
    ( (This)->lpVtbl -> Toggle(This) ) 

#define IUIAutomationTogglePattern_get_CurrentToggleState(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentToggleState(This,retVal) ) 

#define IUIAutomationTogglePattern_get_CachedToggleState(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedToggleState(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationTogglePattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationTransformPattern_INTERFACE_DEFINED__
#define __IUIAutomationTransformPattern_INTERFACE_DEFINED__

/* interface IUIAutomationTransformPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationTransformPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A9B55844-A55D-4EF0-926D-569C16FF89BB")
    IUIAutomationTransformPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Move( 
            /* [in] */ double x,
            /* [in] */ double y) = 0;
        
        virtual HRESULT __stdcall Resize( 
            /* [in] */ double width,
            /* [in] */ double height) = 0;
        
        virtual HRESULT __stdcall Rotate( 
            /* [in] */ double degrees) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentCanMove( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentCanResize( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentCanRotate( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedCanMove( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedCanResize( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedCanRotate( 
            /* [retval][out] */ long *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationTransformPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationTransformPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationTransformPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationTransformPattern * This);
        
        HRESULT ( __stdcall *Move )( 
            IUIAutomationTransformPattern * This,
            /* [in] */ double x,
            /* [in] */ double y);
        
        HRESULT ( __stdcall *Resize )( 
            IUIAutomationTransformPattern * This,
            /* [in] */ double width,
            /* [in] */ double height);
        
        HRESULT ( __stdcall *Rotate )( 
            IUIAutomationTransformPattern * This,
            /* [in] */ double degrees);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentCanMove )( 
            IUIAutomationTransformPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentCanResize )( 
            IUIAutomationTransformPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentCanRotate )( 
            IUIAutomationTransformPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedCanMove )( 
            IUIAutomationTransformPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedCanResize )( 
            IUIAutomationTransformPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedCanRotate )( 
            IUIAutomationTransformPattern * This,
            /* [retval][out] */ long *retVal);
        
        END_INTERFACE
    } IUIAutomationTransformPatternVtbl;

    interface IUIAutomationTransformPattern
    {
        CONST_VTBL struct IUIAutomationTransformPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationTransformPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationTransformPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationTransformPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationTransformPattern_Move(This,x,y)	\
    ( (This)->lpVtbl -> Move(This,x,y) ) 

#define IUIAutomationTransformPattern_Resize(This,width,height)	\
    ( (This)->lpVtbl -> Resize(This,width,height) ) 

#define IUIAutomationTransformPattern_Rotate(This,degrees)	\
    ( (This)->lpVtbl -> Rotate(This,degrees) ) 

#define IUIAutomationTransformPattern_get_CurrentCanMove(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentCanMove(This,retVal) ) 

#define IUIAutomationTransformPattern_get_CurrentCanResize(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentCanResize(This,retVal) ) 

#define IUIAutomationTransformPattern_get_CurrentCanRotate(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentCanRotate(This,retVal) ) 

#define IUIAutomationTransformPattern_get_CachedCanMove(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedCanMove(This,retVal) ) 

#define IUIAutomationTransformPattern_get_CachedCanResize(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedCanResize(This,retVal) ) 

#define IUIAutomationTransformPattern_get_CachedCanRotate(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedCanRotate(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationTransformPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationValuePattern_INTERFACE_DEFINED__
#define __IUIAutomationValuePattern_INTERFACE_DEFINED__

/* interface IUIAutomationValuePattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationValuePattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A94CD8B1-0844-4CD6-9D2D-640537AB39E9")
    IUIAutomationValuePattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall SetValue( 
            /* [in] */ BSTR val) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentValue( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsReadOnly( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedValue( 
            /* [retval][out] */ BSTR *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsReadOnly( 
            /* [retval][out] */ long *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationValuePatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationValuePattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationValuePattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationValuePattern * This);
        
        HRESULT ( __stdcall *SetValue )( 
            IUIAutomationValuePattern * This,
            /* [in] */ BSTR val);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentValue )( 
            IUIAutomationValuePattern * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsReadOnly )( 
            IUIAutomationValuePattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedValue )( 
            IUIAutomationValuePattern * This,
            /* [retval][out] */ BSTR *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsReadOnly )( 
            IUIAutomationValuePattern * This,
            /* [retval][out] */ long *retVal);
        
        END_INTERFACE
    } IUIAutomationValuePatternVtbl;

    interface IUIAutomationValuePattern
    {
        CONST_VTBL struct IUIAutomationValuePatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationValuePattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationValuePattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationValuePattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationValuePattern_SetValue(This,val)	\
    ( (This)->lpVtbl -> SetValue(This,val) ) 

#define IUIAutomationValuePattern_get_CurrentValue(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentValue(This,retVal) ) 

#define IUIAutomationValuePattern_get_CurrentIsReadOnly(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsReadOnly(This,retVal) ) 

#define IUIAutomationValuePattern_get_CachedValue(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedValue(This,retVal) ) 

#define IUIAutomationValuePattern_get_CachedIsReadOnly(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsReadOnly(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationValuePattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationWindowPattern_INTERFACE_DEFINED__
#define __IUIAutomationWindowPattern_INTERFACE_DEFINED__

/* interface IUIAutomationWindowPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationWindowPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0FAEF453-9208-43EF-BBB2-3B485177864F")
    IUIAutomationWindowPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Close( void) = 0;
        
        virtual HRESULT __stdcall WaitForInputIdle( 
            /* [in] */ int milliseconds,
            /* [retval][out] */ long *success) = 0;
        
        virtual HRESULT __stdcall SetWindowVisualState( 
            /* [in] */ WindowVisualState state) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentCanMaximize( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentCanMinimize( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsModal( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentIsTopmost( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentWindowVisualState( 
            /* [retval][out] */ WindowVisualState *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentWindowInteractionState( 
            /* [retval][out] */ WindowInteractionState *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedCanMaximize( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedCanMinimize( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsModal( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedIsTopmost( 
            /* [retval][out] */ long *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedWindowVisualState( 
            /* [retval][out] */ WindowVisualState *retVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedWindowInteractionState( 
            /* [retval][out] */ WindowInteractionState *retVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationWindowPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationWindowPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationWindowPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationWindowPattern * This);
        
        HRESULT ( __stdcall *Close )( 
            IUIAutomationWindowPattern * This);
        
        HRESULT ( __stdcall *WaitForInputIdle )( 
            IUIAutomationWindowPattern * This,
            /* [in] */ int milliseconds,
            /* [retval][out] */ long *success);
        
        HRESULT ( __stdcall *SetWindowVisualState )( 
            IUIAutomationWindowPattern * This,
            /* [in] */ WindowVisualState state);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentCanMaximize )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentCanMinimize )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsModal )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentIsTopmost )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentWindowVisualState )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ WindowVisualState *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentWindowInteractionState )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ WindowInteractionState *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedCanMaximize )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedCanMinimize )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsModal )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedIsTopmost )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ long *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedWindowVisualState )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ WindowVisualState *retVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedWindowInteractionState )( 
            IUIAutomationWindowPattern * This,
            /* [retval][out] */ WindowInteractionState *retVal);
        
        END_INTERFACE
    } IUIAutomationWindowPatternVtbl;

    interface IUIAutomationWindowPattern
    {
        CONST_VTBL struct IUIAutomationWindowPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationWindowPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationWindowPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationWindowPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationWindowPattern_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#define IUIAutomationWindowPattern_WaitForInputIdle(This,milliseconds,success)	\
    ( (This)->lpVtbl -> WaitForInputIdle(This,milliseconds,success) ) 

#define IUIAutomationWindowPattern_SetWindowVisualState(This,state)	\
    ( (This)->lpVtbl -> SetWindowVisualState(This,state) ) 

#define IUIAutomationWindowPattern_get_CurrentCanMaximize(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentCanMaximize(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CurrentCanMinimize(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentCanMinimize(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CurrentIsModal(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsModal(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CurrentIsTopmost(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentIsTopmost(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CurrentWindowVisualState(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentWindowVisualState(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CurrentWindowInteractionState(This,retVal)	\
    ( (This)->lpVtbl -> get_CurrentWindowInteractionState(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CachedCanMaximize(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedCanMaximize(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CachedCanMinimize(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedCanMinimize(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CachedIsModal(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsModal(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CachedIsTopmost(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedIsTopmost(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CachedWindowVisualState(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedWindowVisualState(This,retVal) ) 

#define IUIAutomationWindowPattern_get_CachedWindowInteractionState(This,retVal)	\
    ( (This)->lpVtbl -> get_CachedWindowInteractionState(This,retVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationWindowPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationTextRange_INTERFACE_DEFINED__
#define __IUIAutomationTextRange_INTERFACE_DEFINED__

/* interface IUIAutomationTextRange */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationTextRange;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A543CC6A-F4AE-494B-8239-C814481187A8")
    IUIAutomationTextRange : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Clone( 
            /* [retval][out] */ IUIAutomationTextRange **clonedRange) = 0;
        
        virtual HRESULT __stdcall Compare( 
            /* [in] */ IUIAutomationTextRange *range,
            /* [retval][out] */ long *areSame) = 0;
        
        virtual HRESULT __stdcall CompareEndpoints( 
            /* [in] */ TextPatternRangeEndpoint srcEndPoint,
            /* [in] */ IUIAutomationTextRange *range,
            /* [in] */ TextPatternRangeEndpoint targetEndPoint,
            /* [retval][out] */ int *compValue) = 0;
        
        virtual HRESULT __stdcall ExpandToEnclosingUnit( 
            /* [in] */ TextUnit TextUnit) = 0;
        
        virtual HRESULT __stdcall FindAttribute( 
            /* [in] */ int attr,
            /* [in] */ VARIANT val,
            /* [in] */ long backward,
            /* [retval][out] */ IUIAutomationTextRange **found) = 0;
        
        virtual HRESULT __stdcall FindText( 
            /* [in] */ BSTR text,
            /* [in] */ long backward,
            /* [in] */ long ignoreCase,
            /* [retval][out] */ IUIAutomationTextRange **found) = 0;
        
        virtual HRESULT __stdcall GetAttributeValue( 
            /* [in] */ int attr,
            /* [retval][out] */ VARIANT *value) = 0;
        
        virtual HRESULT __stdcall GetBoundingRectangles( 
            /* [retval][out] */ SAFEARRAY * *boundingRects) = 0;
        
        virtual HRESULT __stdcall GetEnclosingElement( 
            /* [retval][out] */ IUIAutomationElement **enclosingElement) = 0;
        
        virtual HRESULT __stdcall GetText( 
            /* [in] */ int maxLength,
            /* [retval][out] */ BSTR *text) = 0;
        
        virtual HRESULT __stdcall Move( 
            /* [in] */ TextUnit unit,
            /* [in] */ int count,
            /* [retval][out] */ int *moved) = 0;
        
        virtual HRESULT __stdcall MoveEndpointByUnit( 
            /* [in] */ TextPatternRangeEndpoint endpoint,
            /* [in] */ TextUnit unit,
            /* [in] */ int count,
            /* [retval][out] */ int *moved) = 0;
        
        virtual HRESULT __stdcall MoveEndpointByRange( 
            /* [in] */ TextPatternRangeEndpoint srcEndPoint,
            /* [in] */ IUIAutomationTextRange *range,
            /* [in] */ TextPatternRangeEndpoint targetEndPoint) = 0;
        
        virtual HRESULT __stdcall Select( void) = 0;
        
        virtual HRESULT __stdcall AddToSelection( void) = 0;
        
        virtual HRESULT __stdcall RemoveFromSelection( void) = 0;
        
        virtual HRESULT __stdcall ScrollIntoView( 
            /* [in] */ long alignToTop) = 0;
        
        virtual HRESULT __stdcall GetChildren( 
            /* [retval][out] */ IUIAutomationElementArray **children) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationTextRangeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationTextRange * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationTextRange * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationTextRange * This);
        
        HRESULT ( __stdcall *Clone )( 
            IUIAutomationTextRange * This,
            /* [retval][out] */ IUIAutomationTextRange **clonedRange);
        
        HRESULT ( __stdcall *Compare )( 
            IUIAutomationTextRange * This,
            /* [in] */ IUIAutomationTextRange *range,
            /* [retval][out] */ long *areSame);
        
        HRESULT ( __stdcall *CompareEndpoints )( 
            IUIAutomationTextRange * This,
            /* [in] */ TextPatternRangeEndpoint srcEndPoint,
            /* [in] */ IUIAutomationTextRange *range,
            /* [in] */ TextPatternRangeEndpoint targetEndPoint,
            /* [retval][out] */ int *compValue);
        
        HRESULT ( __stdcall *ExpandToEnclosingUnit )( 
            IUIAutomationTextRange * This,
            /* [in] */ TextUnit TextUnit);
        
        HRESULT ( __stdcall *FindAttribute )( 
            IUIAutomationTextRange * This,
            /* [in] */ int attr,
            /* [in] */ VARIANT val,
            /* [in] */ long backward,
            /* [retval][out] */ IUIAutomationTextRange **found);
        
        HRESULT ( __stdcall *FindText )( 
            IUIAutomationTextRange * This,
            /* [in] */ BSTR text,
            /* [in] */ long backward,
            /* [in] */ long ignoreCase,
            /* [retval][out] */ IUIAutomationTextRange **found);
        
        HRESULT ( __stdcall *GetAttributeValue )( 
            IUIAutomationTextRange * This,
            /* [in] */ int attr,
            /* [retval][out] */ VARIANT *value);
        
        HRESULT ( __stdcall *GetBoundingRectangles )( 
            IUIAutomationTextRange * This,
            /* [retval][out] */ SAFEARRAY * *boundingRects);
        
        HRESULT ( __stdcall *GetEnclosingElement )( 
            IUIAutomationTextRange * This,
            /* [retval][out] */ IUIAutomationElement **enclosingElement);
        
        HRESULT ( __stdcall *GetText )( 
            IUIAutomationTextRange * This,
            /* [in] */ int maxLength,
            /* [retval][out] */ BSTR *text);
        
        HRESULT ( __stdcall *Move )( 
            IUIAutomationTextRange * This,
            /* [in] */ TextUnit unit,
            /* [in] */ int count,
            /* [retval][out] */ int *moved);
        
        HRESULT ( __stdcall *MoveEndpointByUnit )( 
            IUIAutomationTextRange * This,
            /* [in] */ TextPatternRangeEndpoint endpoint,
            /* [in] */ TextUnit unit,
            /* [in] */ int count,
            /* [retval][out] */ int *moved);
        
        HRESULT ( __stdcall *MoveEndpointByRange )( 
            IUIAutomationTextRange * This,
            /* [in] */ TextPatternRangeEndpoint srcEndPoint,
            /* [in] */ IUIAutomationTextRange *range,
            /* [in] */ TextPatternRangeEndpoint targetEndPoint);
        
        HRESULT ( __stdcall *Select )( 
            IUIAutomationTextRange * This);
        
        HRESULT ( __stdcall *AddToSelection )( 
            IUIAutomationTextRange * This);
        
        HRESULT ( __stdcall *RemoveFromSelection )( 
            IUIAutomationTextRange * This);
        
        HRESULT ( __stdcall *ScrollIntoView )( 
            IUIAutomationTextRange * This,
            /* [in] */ long alignToTop);
        
        HRESULT ( __stdcall *GetChildren )( 
            IUIAutomationTextRange * This,
            /* [retval][out] */ IUIAutomationElementArray **children);
        
        END_INTERFACE
    } IUIAutomationTextRangeVtbl;

    interface IUIAutomationTextRange
    {
        CONST_VTBL struct IUIAutomationTextRangeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationTextRange_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationTextRange_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationTextRange_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationTextRange_Clone(This,clonedRange)	\
    ( (This)->lpVtbl -> Clone(This,clonedRange) ) 

#define IUIAutomationTextRange_Compare(This,range,areSame)	\
    ( (This)->lpVtbl -> Compare(This,range,areSame) ) 

#define IUIAutomationTextRange_CompareEndpoints(This,srcEndPoint,range,targetEndPoint,compValue)	\
    ( (This)->lpVtbl -> CompareEndpoints(This,srcEndPoint,range,targetEndPoint,compValue) ) 

#define IUIAutomationTextRange_ExpandToEnclosingUnit(This,TextUnit)	\
    ( (This)->lpVtbl -> ExpandToEnclosingUnit(This,TextUnit) ) 

#define IUIAutomationTextRange_FindAttribute(This,attr,val,backward,found)	\
    ( (This)->lpVtbl -> FindAttribute(This,attr,val,backward,found) ) 

#define IUIAutomationTextRange_FindText(This,text,backward,ignoreCase,found)	\
    ( (This)->lpVtbl -> FindText(This,text,backward,ignoreCase,found) ) 

#define IUIAutomationTextRange_GetAttributeValue(This,attr,value)	\
    ( (This)->lpVtbl -> GetAttributeValue(This,attr,value) ) 

#define IUIAutomationTextRange_GetBoundingRectangles(This,boundingRects)	\
    ( (This)->lpVtbl -> GetBoundingRectangles(This,boundingRects) ) 

#define IUIAutomationTextRange_GetEnclosingElement(This,enclosingElement)	\
    ( (This)->lpVtbl -> GetEnclosingElement(This,enclosingElement) ) 

#define IUIAutomationTextRange_GetText(This,maxLength,text)	\
    ( (This)->lpVtbl -> GetText(This,maxLength,text) ) 

#define IUIAutomationTextRange_Move(This,unit,count,moved)	\
    ( (This)->lpVtbl -> Move(This,unit,count,moved) ) 

#define IUIAutomationTextRange_MoveEndpointByUnit(This,endpoint,unit,count,moved)	\
    ( (This)->lpVtbl -> MoveEndpointByUnit(This,endpoint,unit,count,moved) ) 

#define IUIAutomationTextRange_MoveEndpointByRange(This,srcEndPoint,range,targetEndPoint)	\
    ( (This)->lpVtbl -> MoveEndpointByRange(This,srcEndPoint,range,targetEndPoint) ) 

#define IUIAutomationTextRange_Select(This)	\
    ( (This)->lpVtbl -> Select(This) ) 

#define IUIAutomationTextRange_AddToSelection(This)	\
    ( (This)->lpVtbl -> AddToSelection(This) ) 

#define IUIAutomationTextRange_RemoveFromSelection(This)	\
    ( (This)->lpVtbl -> RemoveFromSelection(This) ) 

#define IUIAutomationTextRange_ScrollIntoView(This,alignToTop)	\
    ( (This)->lpVtbl -> ScrollIntoView(This,alignToTop) ) 

#define IUIAutomationTextRange_GetChildren(This,children)	\
    ( (This)->lpVtbl -> GetChildren(This,children) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationTextRange_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationTextRangeArray_INTERFACE_DEFINED__
#define __IUIAutomationTextRangeArray_INTERFACE_DEFINED__

/* interface IUIAutomationTextRangeArray */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationTextRangeArray;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE4AE76A-E717-4C98-81EA-47371D028EB6")
    IUIAutomationTextRangeArray : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_Length( 
            /* [retval][out] */ int *Length) = 0;
        
        virtual HRESULT __stdcall GetElement( 
            /* [in] */ int index,
            /* [retval][out] */ IUIAutomationTextRange **element) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationTextRangeArrayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationTextRangeArray * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationTextRangeArray * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationTextRangeArray * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_Length )( 
            IUIAutomationTextRangeArray * This,
            /* [retval][out] */ int *Length);
        
        HRESULT ( __stdcall *GetElement )( 
            IUIAutomationTextRangeArray * This,
            /* [in] */ int index,
            /* [retval][out] */ IUIAutomationTextRange **element);
        
        END_INTERFACE
    } IUIAutomationTextRangeArrayVtbl;

    interface IUIAutomationTextRangeArray
    {
        CONST_VTBL struct IUIAutomationTextRangeArrayVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationTextRangeArray_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationTextRangeArray_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationTextRangeArray_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationTextRangeArray_get_Length(This,Length)	\
    ( (This)->lpVtbl -> get_Length(This,Length) ) 

#define IUIAutomationTextRangeArray_GetElement(This,index,element)	\
    ( (This)->lpVtbl -> GetElement(This,index,element) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationTextRangeArray_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationTextPattern_INTERFACE_DEFINED__
#define __IUIAutomationTextPattern_INTERFACE_DEFINED__

/* interface IUIAutomationTextPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationTextPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("32EBA289-3583-42C9-9C59-3B6D9A1E9B6A")
    IUIAutomationTextPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall RangeFromPoint( 
            /* [in] */ tagPOINT pt,
            /* [retval][out] */ IUIAutomationTextRange **range) = 0;
        
        virtual HRESULT __stdcall RangeFromChild( 
            /* [in] */ IUIAutomationElement *child,
            /* [retval][out] */ IUIAutomationTextRange **range) = 0;
        
        virtual HRESULT __stdcall GetSelection( 
            /* [retval][out] */ IUIAutomationTextRangeArray **ranges) = 0;
        
        virtual HRESULT __stdcall GetVisibleRanges( 
            /* [retval][out] */ IUIAutomationTextRangeArray **ranges) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_DocumentRange( 
            /* [retval][out] */ IUIAutomationTextRange **range) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_SupportedTextSelection( 
            /* [retval][out] */ SupportedTextSelection *SupportedTextSelection) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationTextPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationTextPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationTextPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationTextPattern * This);
        
        HRESULT ( __stdcall *RangeFromPoint )( 
            IUIAutomationTextPattern * This,
            /* [in] */ tagPOINT pt,
            /* [retval][out] */ IUIAutomationTextRange **range);
        
        HRESULT ( __stdcall *RangeFromChild )( 
            IUIAutomationTextPattern * This,
            /* [in] */ IUIAutomationElement *child,
            /* [retval][out] */ IUIAutomationTextRange **range);
        
        HRESULT ( __stdcall *GetSelection )( 
            IUIAutomationTextPattern * This,
            /* [retval][out] */ IUIAutomationTextRangeArray **ranges);
        
        HRESULT ( __stdcall *GetVisibleRanges )( 
            IUIAutomationTextPattern * This,
            /* [retval][out] */ IUIAutomationTextRangeArray **ranges);
        
        /* [propget] */ HRESULT ( __stdcall *get_DocumentRange )( 
            IUIAutomationTextPattern * This,
            /* [retval][out] */ IUIAutomationTextRange **range);
        
        /* [propget] */ HRESULT ( __stdcall *get_SupportedTextSelection )( 
            IUIAutomationTextPattern * This,
            /* [retval][out] */ SupportedTextSelection *SupportedTextSelection);
        
        END_INTERFACE
    } IUIAutomationTextPatternVtbl;

    interface IUIAutomationTextPattern
    {
        CONST_VTBL struct IUIAutomationTextPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationTextPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationTextPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationTextPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationTextPattern_RangeFromPoint(This,pt,range)	\
    ( (This)->lpVtbl -> RangeFromPoint(This,pt,range) ) 

#define IUIAutomationTextPattern_RangeFromChild(This,child,range)	\
    ( (This)->lpVtbl -> RangeFromChild(This,child,range) ) 

#define IUIAutomationTextPattern_GetSelection(This,ranges)	\
    ( (This)->lpVtbl -> GetSelection(This,ranges) ) 

#define IUIAutomationTextPattern_GetVisibleRanges(This,ranges)	\
    ( (This)->lpVtbl -> GetVisibleRanges(This,ranges) ) 

#define IUIAutomationTextPattern_get_DocumentRange(This,range)	\
    ( (This)->lpVtbl -> get_DocumentRange(This,range) ) 

#define IUIAutomationTextPattern_get_SupportedTextSelection(This,SupportedTextSelection)	\
    ( (This)->lpVtbl -> get_SupportedTextSelection(This,SupportedTextSelection) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationTextPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationLegacyIAccessiblePattern_INTERFACE_DEFINED__
#define __IUIAutomationLegacyIAccessiblePattern_INTERFACE_DEFINED__

/* interface IUIAutomationLegacyIAccessiblePattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationLegacyIAccessiblePattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("828055AD-355B-4435-86D5-3B51C14A9B1B")
    IUIAutomationLegacyIAccessiblePattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Select( 
            long flagsSelect) = 0;
        
        virtual HRESULT __stdcall DoDefaultAction( void) = 0;
        
        virtual HRESULT __stdcall SetValue( 
            LPWSTR szValue) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentChildId( 
            /* [retval][out] */ int *pRetVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentName( 
            /* [retval][out] */ BSTR *pszName) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentValue( 
            /* [retval][out] */ BSTR *pszValue) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentDescription( 
            /* [retval][out] */ BSTR *pszDescription) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentRole( 
            /* [retval][out] */ unsigned long *pdwRole) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentState( 
            /* [retval][out] */ unsigned long *pdwState) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentHelp( 
            /* [retval][out] */ BSTR *pszHelp) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentKeyboardShortcut( 
            /* [retval][out] */ BSTR *pszKeyboardShortcut) = 0;
        
        virtual HRESULT __stdcall GetCurrentSelection( 
            /* [retval][out] */ IUIAutomationElementArray **pvarSelectedChildren) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CurrentDefaultAction( 
            /* [retval][out] */ BSTR *pszDefaultAction) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedChildId( 
            /* [retval][out] */ int *pRetVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedName( 
            /* [retval][out] */ BSTR *pszName) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedValue( 
            /* [retval][out] */ BSTR *pszValue) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedDescription( 
            /* [retval][out] */ BSTR *pszDescription) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedRole( 
            /* [retval][out] */ unsigned long *pdwRole) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedState( 
            /* [retval][out] */ unsigned long *pdwState) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedHelp( 
            /* [retval][out] */ BSTR *pszHelp) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedKeyboardShortcut( 
            /* [retval][out] */ BSTR *pszKeyboardShortcut) = 0;
        
        virtual HRESULT __stdcall GetCachedSelection( 
            /* [retval][out] */ IUIAutomationElementArray **pvarSelectedChildren) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CachedDefaultAction( 
            /* [retval][out] */ BSTR *pszDefaultAction) = 0;
        
        virtual HRESULT __stdcall GetIAccessible( 
            /* [retval][out] */ IAccessible **ppAccessible) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationLegacyIAccessiblePatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationLegacyIAccessiblePattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationLegacyIAccessiblePattern * This);
        
        HRESULT ( __stdcall *Select )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            long flagsSelect);
        
        HRESULT ( __stdcall *DoDefaultAction )( 
            IUIAutomationLegacyIAccessiblePattern * This);
        
        HRESULT ( __stdcall *SetValue )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            LPWSTR szValue);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentChildId )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ int *pRetVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentName )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszName);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentValue )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszValue);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentDescription )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszDescription);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentRole )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ unsigned long *pdwRole);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentState )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ unsigned long *pdwState);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentHelp )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszHelp);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentKeyboardShortcut )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszKeyboardShortcut);
        
        HRESULT ( __stdcall *GetCurrentSelection )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ IUIAutomationElementArray **pvarSelectedChildren);
        
        /* [propget] */ HRESULT ( __stdcall *get_CurrentDefaultAction )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszDefaultAction);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedChildId )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ int *pRetVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedName )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszName);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedValue )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszValue);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedDescription )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszDescription);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedRole )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ unsigned long *pdwRole);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedState )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ unsigned long *pdwState);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedHelp )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszHelp);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedKeyboardShortcut )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszKeyboardShortcut);
        
        HRESULT ( __stdcall *GetCachedSelection )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ IUIAutomationElementArray **pvarSelectedChildren);
        
        /* [propget] */ HRESULT ( __stdcall *get_CachedDefaultAction )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ BSTR *pszDefaultAction);
        
        HRESULT ( __stdcall *GetIAccessible )( 
            IUIAutomationLegacyIAccessiblePattern * This,
            /* [retval][out] */ IAccessible **ppAccessible);
        
        END_INTERFACE
    } IUIAutomationLegacyIAccessiblePatternVtbl;

    interface IUIAutomationLegacyIAccessiblePattern
    {
        CONST_VTBL struct IUIAutomationLegacyIAccessiblePatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationLegacyIAccessiblePattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationLegacyIAccessiblePattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationLegacyIAccessiblePattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationLegacyIAccessiblePattern_Select(This,flagsSelect)	\
    ( (This)->lpVtbl -> Select(This,flagsSelect) ) 

#define IUIAutomationLegacyIAccessiblePattern_DoDefaultAction(This)	\
    ( (This)->lpVtbl -> DoDefaultAction(This) ) 

#define IUIAutomationLegacyIAccessiblePattern_SetValue(This,szValue)	\
    ( (This)->lpVtbl -> SetValue(This,szValue) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentChildId(This,pRetVal)	\
    ( (This)->lpVtbl -> get_CurrentChildId(This,pRetVal) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentName(This,pszName)	\
    ( (This)->lpVtbl -> get_CurrentName(This,pszName) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentValue(This,pszValue)	\
    ( (This)->lpVtbl -> get_CurrentValue(This,pszValue) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentDescription(This,pszDescription)	\
    ( (This)->lpVtbl -> get_CurrentDescription(This,pszDescription) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentRole(This,pdwRole)	\
    ( (This)->lpVtbl -> get_CurrentRole(This,pdwRole) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentState(This,pdwState)	\
    ( (This)->lpVtbl -> get_CurrentState(This,pdwState) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentHelp(This,pszHelp)	\
    ( (This)->lpVtbl -> get_CurrentHelp(This,pszHelp) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentKeyboardShortcut(This,pszKeyboardShortcut)	\
    ( (This)->lpVtbl -> get_CurrentKeyboardShortcut(This,pszKeyboardShortcut) ) 

#define IUIAutomationLegacyIAccessiblePattern_GetCurrentSelection(This,pvarSelectedChildren)	\
    ( (This)->lpVtbl -> GetCurrentSelection(This,pvarSelectedChildren) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CurrentDefaultAction(This,pszDefaultAction)	\
    ( (This)->lpVtbl -> get_CurrentDefaultAction(This,pszDefaultAction) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedChildId(This,pRetVal)	\
    ( (This)->lpVtbl -> get_CachedChildId(This,pRetVal) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedName(This,pszName)	\
    ( (This)->lpVtbl -> get_CachedName(This,pszName) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedValue(This,pszValue)	\
    ( (This)->lpVtbl -> get_CachedValue(This,pszValue) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedDescription(This,pszDescription)	\
    ( (This)->lpVtbl -> get_CachedDescription(This,pszDescription) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedRole(This,pdwRole)	\
    ( (This)->lpVtbl -> get_CachedRole(This,pdwRole) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedState(This,pdwState)	\
    ( (This)->lpVtbl -> get_CachedState(This,pdwState) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedHelp(This,pszHelp)	\
    ( (This)->lpVtbl -> get_CachedHelp(This,pszHelp) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedKeyboardShortcut(This,pszKeyboardShortcut)	\
    ( (This)->lpVtbl -> get_CachedKeyboardShortcut(This,pszKeyboardShortcut) ) 

#define IUIAutomationLegacyIAccessiblePattern_GetCachedSelection(This,pvarSelectedChildren)	\
    ( (This)->lpVtbl -> GetCachedSelection(This,pvarSelectedChildren) ) 

#define IUIAutomationLegacyIAccessiblePattern_get_CachedDefaultAction(This,pszDefaultAction)	\
    ( (This)->lpVtbl -> get_CachedDefaultAction(This,pszDefaultAction) ) 

#define IUIAutomationLegacyIAccessiblePattern_GetIAccessible(This,ppAccessible)	\
    ( (This)->lpVtbl -> GetIAccessible(This,ppAccessible) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationLegacyIAccessiblePattern_INTERFACE_DEFINED__ */


#ifndef __IAccessible_INTERFACE_DEFINED__
#define __IAccessible_INTERFACE_DEFINED__

/* interface IAccessible */
/* [object][oleautomation][dual][hidden][uuid] */ 


EXTERN_C const IID IID_IAccessible;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("618736E0-3C3D-11CF-810C-00AA00389B71")
    IAccessible : public IDispatch
    {
    public:
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accParent( 
            /* [retval][out] */ IDispatch **ppdispParent) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accChildCount( 
            /* [retval][out] */ long *pcountChildren) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accChild( 
            /* [in] */ VARIANT varChild,
            /* [retval][out] */ IDispatch **ppdispChild) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accName( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszName) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accValue( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszValue) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accDescription( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszDescription) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accRole( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ VARIANT *pvarRole) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accState( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ VARIANT *pvarState) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accHelp( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszHelp) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accHelpTopic( 
            /* [out] */ BSTR *pszHelpFile,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ long *pidTopic) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszKeyboardShortcut) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accFocus( 
            /* [retval][out] */ VARIANT *pvarChild) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accSelection( 
            /* [retval][out] */ VARIANT *pvarChildren) = 0;
        
        virtual /* [hidden][propget][id] */ HRESULT STDMETHODCALLTYPE get_accDefaultAction( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszDefaultAction) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE accSelect( 
            /* [in] */ long flagsSelect,
            /* [optional][in] */ VARIANT varChild) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE accLocation( 
            /* [out] */ long *pxLeft,
            /* [out] */ long *pyTop,
            /* [out] */ long *pcxWidth,
            /* [out] */ long *pcyHeight,
            /* [optional][in] */ VARIANT varChild) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE accNavigate( 
            /* [in] */ long navDir,
            /* [optional][in] */ VARIANT varStart,
            /* [retval][out] */ VARIANT *pvarEndUpAt) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE accHitTest( 
            /* [in] */ long xLeft,
            /* [in] */ long yTop,
            /* [retval][out] */ VARIANT *pvarChild) = 0;
        
        virtual /* [hidden][id] */ HRESULT STDMETHODCALLTYPE accDoDefaultAction( 
            /* [optional][in] */ VARIANT varChild) = 0;
        
        virtual /* [hidden][propput][id] */ HRESULT STDMETHODCALLTYPE put_accName( 
            /* [optional][in] */ VARIANT varChild,
            /* [in] */ BSTR pszName) = 0;
        
        virtual /* [hidden][propput][id] */ HRESULT STDMETHODCALLTYPE put_accValue( 
            /* [optional][in] */ VARIANT varChild,
            /* [in] */ BSTR pszValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IAccessibleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAccessible * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAccessible * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAccessible * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAccessible * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAccessible * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAccessible * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAccessible * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accParent )( 
            IAccessible * This,
            /* [retval][out] */ IDispatch **ppdispParent);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accChildCount )( 
            IAccessible * This,
            /* [retval][out] */ long *pcountChildren);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accChild )( 
            IAccessible * This,
            /* [in] */ VARIANT varChild,
            /* [retval][out] */ IDispatch **ppdispChild);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accName )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszName);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accValue )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszValue);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accDescription )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszDescription);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accRole )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ VARIANT *pvarRole);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accState )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ VARIANT *pvarState);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accHelp )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszHelp);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accHelpTopic )( 
            IAccessible * This,
            /* [out] */ BSTR *pszHelpFile,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ long *pidTopic);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accKeyboardShortcut )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszKeyboardShortcut);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accFocus )( 
            IAccessible * This,
            /* [retval][out] */ VARIANT *pvarChild);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accSelection )( 
            IAccessible * This,
            /* [retval][out] */ VARIANT *pvarChildren);
        
        /* [hidden][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_accDefaultAction )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszDefaultAction);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *accSelect )( 
            IAccessible * This,
            /* [in] */ long flagsSelect,
            /* [optional][in] */ VARIANT varChild);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *accLocation )( 
            IAccessible * This,
            /* [out] */ long *pxLeft,
            /* [out] */ long *pyTop,
            /* [out] */ long *pcxWidth,
            /* [out] */ long *pcyHeight,
            /* [optional][in] */ VARIANT varChild);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *accNavigate )( 
            IAccessible * This,
            /* [in] */ long navDir,
            /* [optional][in] */ VARIANT varStart,
            /* [retval][out] */ VARIANT *pvarEndUpAt);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *accHitTest )( 
            IAccessible * This,
            /* [in] */ long xLeft,
            /* [in] */ long yTop,
            /* [retval][out] */ VARIANT *pvarChild);
        
        /* [hidden][id] */ HRESULT ( STDMETHODCALLTYPE *accDoDefaultAction )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild);
        
        /* [hidden][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_accName )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [in] */ BSTR pszName);
        
        /* [hidden][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_accValue )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [in] */ BSTR pszValue);
        
        END_INTERFACE
    } IAccessibleVtbl;

    interface IAccessible
    {
        CONST_VTBL struct IAccessibleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAccessible_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAccessible_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAccessible_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAccessible_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAccessible_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAccessible_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAccessible_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IAccessible_get_accParent(This,ppdispParent)	\
    ( (This)->lpVtbl -> get_accParent(This,ppdispParent) ) 

#define IAccessible_get_accChildCount(This,pcountChildren)	\
    ( (This)->lpVtbl -> get_accChildCount(This,pcountChildren) ) 

#define IAccessible_get_accChild(This,varChild,ppdispChild)	\
    ( (This)->lpVtbl -> get_accChild(This,varChild,ppdispChild) ) 

#define IAccessible_get_accName(This,varChild,pszName)	\
    ( (This)->lpVtbl -> get_accName(This,varChild,pszName) ) 

#define IAccessible_get_accValue(This,varChild,pszValue)	\
    ( (This)->lpVtbl -> get_accValue(This,varChild,pszValue) ) 

#define IAccessible_get_accDescription(This,varChild,pszDescription)	\
    ( (This)->lpVtbl -> get_accDescription(This,varChild,pszDescription) ) 

#define IAccessible_get_accRole(This,varChild,pvarRole)	\
    ( (This)->lpVtbl -> get_accRole(This,varChild,pvarRole) ) 

#define IAccessible_get_accState(This,varChild,pvarState)	\
    ( (This)->lpVtbl -> get_accState(This,varChild,pvarState) ) 

#define IAccessible_get_accHelp(This,varChild,pszHelp)	\
    ( (This)->lpVtbl -> get_accHelp(This,varChild,pszHelp) ) 

#define IAccessible_get_accHelpTopic(This,pszHelpFile,varChild,pidTopic)	\
    ( (This)->lpVtbl -> get_accHelpTopic(This,pszHelpFile,varChild,pidTopic) ) 

#define IAccessible_get_accKeyboardShortcut(This,varChild,pszKeyboardShortcut)	\
    ( (This)->lpVtbl -> get_accKeyboardShortcut(This,varChild,pszKeyboardShortcut) ) 

#define IAccessible_get_accFocus(This,pvarChild)	\
    ( (This)->lpVtbl -> get_accFocus(This,pvarChild) ) 

#define IAccessible_get_accSelection(This,pvarChildren)	\
    ( (This)->lpVtbl -> get_accSelection(This,pvarChildren) ) 

#define IAccessible_get_accDefaultAction(This,varChild,pszDefaultAction)	\
    ( (This)->lpVtbl -> get_accDefaultAction(This,varChild,pszDefaultAction) ) 

#define IAccessible_accSelect(This,flagsSelect,varChild)	\
    ( (This)->lpVtbl -> accSelect(This,flagsSelect,varChild) ) 

#define IAccessible_accLocation(This,pxLeft,pyTop,pcxWidth,pcyHeight,varChild)	\
    ( (This)->lpVtbl -> accLocation(This,pxLeft,pyTop,pcxWidth,pcyHeight,varChild) ) 

#define IAccessible_accNavigate(This,navDir,varStart,pvarEndUpAt)	\
    ( (This)->lpVtbl -> accNavigate(This,navDir,varStart,pvarEndUpAt) ) 

#define IAccessible_accHitTest(This,xLeft,yTop,pvarChild)	\
    ( (This)->lpVtbl -> accHitTest(This,xLeft,yTop,pvarChild) ) 

#define IAccessible_accDoDefaultAction(This,varChild)	\
    ( (This)->lpVtbl -> accDoDefaultAction(This,varChild) ) 

#define IAccessible_put_accName(This,varChild,pszName)	\
    ( (This)->lpVtbl -> put_accName(This,varChild,pszName) ) 

#define IAccessible_put_accValue(This,varChild,pszValue)	\
    ( (This)->lpVtbl -> put_accValue(This,varChild,pszValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAccessible_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationItemContainerPattern_INTERFACE_DEFINED__
#define __IUIAutomationItemContainerPattern_INTERFACE_DEFINED__

/* interface IUIAutomationItemContainerPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationItemContainerPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C690FDB2-27A8-423C-812D-429773C9084E")
    IUIAutomationItemContainerPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall FindItemByProperty( 
            /* [in] */ IUIAutomationElement *pStartAfter,
            /* [in] */ int propertyId,
            /* [in] */ VARIANT value,
            /* [retval][out] */ IUIAutomationElement **pFound) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationItemContainerPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationItemContainerPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationItemContainerPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationItemContainerPattern * This);
        
        HRESULT ( __stdcall *FindItemByProperty )( 
            IUIAutomationItemContainerPattern * This,
            /* [in] */ IUIAutomationElement *pStartAfter,
            /* [in] */ int propertyId,
            /* [in] */ VARIANT value,
            /* [retval][out] */ IUIAutomationElement **pFound);
        
        END_INTERFACE
    } IUIAutomationItemContainerPatternVtbl;

    interface IUIAutomationItemContainerPattern
    {
        CONST_VTBL struct IUIAutomationItemContainerPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationItemContainerPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationItemContainerPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationItemContainerPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationItemContainerPattern_FindItemByProperty(This,pStartAfter,propertyId,value,pFound)	\
    ( (This)->lpVtbl -> FindItemByProperty(This,pStartAfter,propertyId,value,pFound) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationItemContainerPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationVirtualizedItemPattern_INTERFACE_DEFINED__
#define __IUIAutomationVirtualizedItemPattern_INTERFACE_DEFINED__

/* interface IUIAutomationVirtualizedItemPattern */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationVirtualizedItemPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6BA3D7A6-04CF-4F11-8793-A8D1CDE9969F")
    IUIAutomationVirtualizedItemPattern : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Realize( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationVirtualizedItemPatternVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationVirtualizedItemPattern * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationVirtualizedItemPattern * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationVirtualizedItemPattern * This);
        
        HRESULT ( __stdcall *Realize )( 
            IUIAutomationVirtualizedItemPattern * This);
        
        END_INTERFACE
    } IUIAutomationVirtualizedItemPatternVtbl;

    interface IUIAutomationVirtualizedItemPattern
    {
        CONST_VTBL struct IUIAutomationVirtualizedItemPatternVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationVirtualizedItemPattern_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationVirtualizedItemPattern_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationVirtualizedItemPattern_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationVirtualizedItemPattern_Realize(This)	\
    ( (This)->lpVtbl -> Realize(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationVirtualizedItemPattern_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationProxyFactory_INTERFACE_DEFINED__
#define __IUIAutomationProxyFactory_INTERFACE_DEFINED__

/* interface IUIAutomationProxyFactory */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationProxyFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("85B94ECD-849D-42B6-B94D-D6DB23FDF5A4")
    IUIAutomationProxyFactory : public IUnknown
    {
    public:
        virtual HRESULT __stdcall CreateProvider( 
            /* [in] */ void *hwnd,
            /* [in] */ long idObject,
            /* [in] */ long idChild,
            /* [retval][out] */ IRawElementProviderSimple **provider) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ProxyFactoryId( 
            /* [retval][out] */ BSTR *factoryId) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationProxyFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationProxyFactory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationProxyFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationProxyFactory * This);
        
        HRESULT ( __stdcall *CreateProvider )( 
            IUIAutomationProxyFactory * This,
            /* [in] */ void *hwnd,
            /* [in] */ long idObject,
            /* [in] */ long idChild,
            /* [retval][out] */ IRawElementProviderSimple **provider);
        
        /* [propget] */ HRESULT ( __stdcall *get_ProxyFactoryId )( 
            IUIAutomationProxyFactory * This,
            /* [retval][out] */ BSTR *factoryId);
        
        END_INTERFACE
    } IUIAutomationProxyFactoryVtbl;

    interface IUIAutomationProxyFactory
    {
        CONST_VTBL struct IUIAutomationProxyFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationProxyFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationProxyFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationProxyFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationProxyFactory_CreateProvider(This,hwnd,idObject,idChild,provider)	\
    ( (This)->lpVtbl -> CreateProvider(This,hwnd,idObject,idChild,provider) ) 

#define IUIAutomationProxyFactory_get_ProxyFactoryId(This,factoryId)	\
    ( (This)->lpVtbl -> get_ProxyFactoryId(This,factoryId) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationProxyFactory_INTERFACE_DEFINED__ */


#ifndef __IRawElementProviderSimple_INTERFACE_DEFINED__
#define __IRawElementProviderSimple_INTERFACE_DEFINED__

/* interface IRawElementProviderSimple */
/* [object][uuid] */ 


EXTERN_C const IID IID_IRawElementProviderSimple;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D6DD68D1-86FD-4332-8666-9ABEDEA2D24C")
    IRawElementProviderSimple : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_ProviderOptions( 
            /* [retval][out] */ ProviderOptions *pRetVal) = 0;
        
        virtual HRESULT __stdcall GetPatternProvider( 
            /* [in] */ int patternId,
            /* [retval][out] */ IUnknown **pRetVal) = 0;
        
        virtual HRESULT __stdcall GetPropertyValue( 
            /* [in] */ int propertyId,
            /* [retval][out] */ VARIANT *pRetVal) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_HostRawElementProvider( 
            /* [retval][out] */ IRawElementProviderSimple **pRetVal) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IRawElementProviderSimpleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRawElementProviderSimple * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRawElementProviderSimple * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRawElementProviderSimple * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_ProviderOptions )( 
            IRawElementProviderSimple * This,
            /* [retval][out] */ ProviderOptions *pRetVal);
        
        HRESULT ( __stdcall *GetPatternProvider )( 
            IRawElementProviderSimple * This,
            /* [in] */ int patternId,
            /* [retval][out] */ IUnknown **pRetVal);
        
        HRESULT ( __stdcall *GetPropertyValue )( 
            IRawElementProviderSimple * This,
            /* [in] */ int propertyId,
            /* [retval][out] */ VARIANT *pRetVal);
        
        /* [propget] */ HRESULT ( __stdcall *get_HostRawElementProvider )( 
            IRawElementProviderSimple * This,
            /* [retval][out] */ IRawElementProviderSimple **pRetVal);
        
        END_INTERFACE
    } IRawElementProviderSimpleVtbl;

    interface IRawElementProviderSimple
    {
        CONST_VTBL struct IRawElementProviderSimpleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRawElementProviderSimple_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IRawElementProviderSimple_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IRawElementProviderSimple_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IRawElementProviderSimple_get_ProviderOptions(This,pRetVal)	\
    ( (This)->lpVtbl -> get_ProviderOptions(This,pRetVal) ) 

#define IRawElementProviderSimple_GetPatternProvider(This,patternId,pRetVal)	\
    ( (This)->lpVtbl -> GetPatternProvider(This,patternId,pRetVal) ) 

#define IRawElementProviderSimple_GetPropertyValue(This,propertyId,pRetVal)	\
    ( (This)->lpVtbl -> GetPropertyValue(This,propertyId,pRetVal) ) 

#define IRawElementProviderSimple_get_HostRawElementProvider(This,pRetVal)	\
    ( (This)->lpVtbl -> get_HostRawElementProvider(This,pRetVal) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IRawElementProviderSimple_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationProxyFactoryEntry_INTERFACE_DEFINED__
#define __IUIAutomationProxyFactoryEntry_INTERFACE_DEFINED__

/* interface IUIAutomationProxyFactoryEntry */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationProxyFactoryEntry;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D50E472E-B64B-490C-BCA1-D30696F9F289")
    IUIAutomationProxyFactoryEntry : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_ProxyFactory( 
            /* [retval][out] */ IUIAutomationProxyFactory **factory) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ClassName( 
            /* [retval][out] */ BSTR *ClassName) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ImageName( 
            /* [retval][out] */ BSTR *ImageName) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_AllowSubstringMatch( 
            /* [retval][out] */ long *AllowSubstringMatch) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_CanCheckBaseClass( 
            /* [retval][out] */ long *CanCheckBaseClass) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_NeedsAdviseEvents( 
            /* [retval][out] */ long *adviseEvents) = 0;
        
        virtual /* [propput] */ HRESULT __stdcall put_ClassName( 
            /* [in] */ LPWSTR ClassName) = 0;
        
        virtual /* [propput] */ HRESULT __stdcall put_ImageName( 
            /* [in] */ LPWSTR ImageName) = 0;
        
        virtual /* [propput] */ HRESULT __stdcall put_AllowSubstringMatch( 
            /* [in] */ long AllowSubstringMatch) = 0;
        
        virtual /* [propput] */ HRESULT __stdcall put_CanCheckBaseClass( 
            /* [in] */ long CanCheckBaseClass) = 0;
        
        virtual /* [propput] */ HRESULT __stdcall put_NeedsAdviseEvents( 
            /* [in] */ long adviseEvents) = 0;
        
        virtual HRESULT __stdcall SetWinEventsForAutomationEvent( 
            /* [in] */ int eventId,
            /* [in] */ int propertyId,
            /* [in] */ SAFEARRAY * winEvents) = 0;
        
        virtual HRESULT __stdcall GetWinEventsForAutomationEvent( 
            /* [in] */ int eventId,
            /* [in] */ int propertyId,
            /* [retval][out] */ SAFEARRAY * *winEvents) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationProxyFactoryEntryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationProxyFactoryEntry * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationProxyFactoryEntry * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_ProxyFactory )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [retval][out] */ IUIAutomationProxyFactory **factory);
        
        /* [propget] */ HRESULT ( __stdcall *get_ClassName )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [retval][out] */ BSTR *ClassName);
        
        /* [propget] */ HRESULT ( __stdcall *get_ImageName )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [retval][out] */ BSTR *ImageName);
        
        /* [propget] */ HRESULT ( __stdcall *get_AllowSubstringMatch )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [retval][out] */ long *AllowSubstringMatch);
        
        /* [propget] */ HRESULT ( __stdcall *get_CanCheckBaseClass )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [retval][out] */ long *CanCheckBaseClass);
        
        /* [propget] */ HRESULT ( __stdcall *get_NeedsAdviseEvents )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [retval][out] */ long *adviseEvents);
        
        /* [propput] */ HRESULT ( __stdcall *put_ClassName )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [in] */ LPWSTR ClassName);
        
        /* [propput] */ HRESULT ( __stdcall *put_ImageName )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [in] */ LPWSTR ImageName);
        
        /* [propput] */ HRESULT ( __stdcall *put_AllowSubstringMatch )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [in] */ long AllowSubstringMatch);
        
        /* [propput] */ HRESULT ( __stdcall *put_CanCheckBaseClass )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [in] */ long CanCheckBaseClass);
        
        /* [propput] */ HRESULT ( __stdcall *put_NeedsAdviseEvents )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [in] */ long adviseEvents);
        
        HRESULT ( __stdcall *SetWinEventsForAutomationEvent )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [in] */ int eventId,
            /* [in] */ int propertyId,
            /* [in] */ SAFEARRAY * winEvents);
        
        HRESULT ( __stdcall *GetWinEventsForAutomationEvent )( 
            IUIAutomationProxyFactoryEntry * This,
            /* [in] */ int eventId,
            /* [in] */ int propertyId,
            /* [retval][out] */ SAFEARRAY * *winEvents);
        
        END_INTERFACE
    } IUIAutomationProxyFactoryEntryVtbl;

    interface IUIAutomationProxyFactoryEntry
    {
        CONST_VTBL struct IUIAutomationProxyFactoryEntryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationProxyFactoryEntry_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationProxyFactoryEntry_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationProxyFactoryEntry_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationProxyFactoryEntry_get_ProxyFactory(This,factory)	\
    ( (This)->lpVtbl -> get_ProxyFactory(This,factory) ) 

#define IUIAutomationProxyFactoryEntry_get_ClassName(This,ClassName)	\
    ( (This)->lpVtbl -> get_ClassName(This,ClassName) ) 

#define IUIAutomationProxyFactoryEntry_get_ImageName(This,ImageName)	\
    ( (This)->lpVtbl -> get_ImageName(This,ImageName) ) 

#define IUIAutomationProxyFactoryEntry_get_AllowSubstringMatch(This,AllowSubstringMatch)	\
    ( (This)->lpVtbl -> get_AllowSubstringMatch(This,AllowSubstringMatch) ) 

#define IUIAutomationProxyFactoryEntry_get_CanCheckBaseClass(This,CanCheckBaseClass)	\
    ( (This)->lpVtbl -> get_CanCheckBaseClass(This,CanCheckBaseClass) ) 

#define IUIAutomationProxyFactoryEntry_get_NeedsAdviseEvents(This,adviseEvents)	\
    ( (This)->lpVtbl -> get_NeedsAdviseEvents(This,adviseEvents) ) 

#define IUIAutomationProxyFactoryEntry_put_ClassName(This,ClassName)	\
    ( (This)->lpVtbl -> put_ClassName(This,ClassName) ) 

#define IUIAutomationProxyFactoryEntry_put_ImageName(This,ImageName)	\
    ( (This)->lpVtbl -> put_ImageName(This,ImageName) ) 

#define IUIAutomationProxyFactoryEntry_put_AllowSubstringMatch(This,AllowSubstringMatch)	\
    ( (This)->lpVtbl -> put_AllowSubstringMatch(This,AllowSubstringMatch) ) 

#define IUIAutomationProxyFactoryEntry_put_CanCheckBaseClass(This,CanCheckBaseClass)	\
    ( (This)->lpVtbl -> put_CanCheckBaseClass(This,CanCheckBaseClass) ) 

#define IUIAutomationProxyFactoryEntry_put_NeedsAdviseEvents(This,adviseEvents)	\
    ( (This)->lpVtbl -> put_NeedsAdviseEvents(This,adviseEvents) ) 

#define IUIAutomationProxyFactoryEntry_SetWinEventsForAutomationEvent(This,eventId,propertyId,winEvents)	\
    ( (This)->lpVtbl -> SetWinEventsForAutomationEvent(This,eventId,propertyId,winEvents) ) 

#define IUIAutomationProxyFactoryEntry_GetWinEventsForAutomationEvent(This,eventId,propertyId,winEvents)	\
    ( (This)->lpVtbl -> GetWinEventsForAutomationEvent(This,eventId,propertyId,winEvents) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationProxyFactoryEntry_INTERFACE_DEFINED__ */


#ifndef __IUIAutomationProxyFactoryMapping_INTERFACE_DEFINED__
#define __IUIAutomationProxyFactoryMapping_INTERFACE_DEFINED__

/* interface IUIAutomationProxyFactoryMapping */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomationProxyFactoryMapping;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("09E31E18-872D-4873-93D1-1E541EC133FD")
    IUIAutomationProxyFactoryMapping : public IUnknown
    {
    public:
        virtual /* [propget] */ HRESULT __stdcall get_count( 
            /* [retval][out] */ unsigned int *count) = 0;
        
        virtual HRESULT __stdcall GetTable( 
            /* [retval][out] */ SAFEARRAY * *table) = 0;
        
        virtual HRESULT __stdcall GetEntry( 
            /* [in] */ unsigned int index,
            /* [retval][out] */ IUIAutomationProxyFactoryEntry **entry) = 0;
        
        virtual HRESULT __stdcall SetTable( 
            /* [in] */ SAFEARRAY * factoryList) = 0;
        
        virtual HRESULT __stdcall InsertEntries( 
            /* [in] */ unsigned int before,
            /* [in] */ SAFEARRAY * factoryList) = 0;
        
        virtual HRESULT __stdcall InsertEntry( 
            /* [in] */ unsigned int before,
            /* [in] */ IUIAutomationProxyFactoryEntry *factory) = 0;
        
        virtual HRESULT __stdcall RemoveEntry( 
            /* [in] */ unsigned int index) = 0;
        
        virtual HRESULT __stdcall ClearTable( void) = 0;
        
        virtual HRESULT __stdcall RestoreDefaultTable( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationProxyFactoryMappingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomationProxyFactoryMapping * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomationProxyFactoryMapping * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomationProxyFactoryMapping * This);
        
        /* [propget] */ HRESULT ( __stdcall *get_count )( 
            IUIAutomationProxyFactoryMapping * This,
            /* [retval][out] */ unsigned int *count);
        
        HRESULT ( __stdcall *GetTable )( 
            IUIAutomationProxyFactoryMapping * This,
            /* [retval][out] */ SAFEARRAY * *table);
        
        HRESULT ( __stdcall *GetEntry )( 
            IUIAutomationProxyFactoryMapping * This,
            /* [in] */ unsigned int index,
            /* [retval][out] */ IUIAutomationProxyFactoryEntry **entry);
        
        HRESULT ( __stdcall *SetTable )( 
            IUIAutomationProxyFactoryMapping * This,
            /* [in] */ SAFEARRAY * factoryList);
        
        HRESULT ( __stdcall *InsertEntries )( 
            IUIAutomationProxyFactoryMapping * This,
            /* [in] */ unsigned int before,
            /* [in] */ SAFEARRAY * factoryList);
        
        HRESULT ( __stdcall *InsertEntry )( 
            IUIAutomationProxyFactoryMapping * This,
            /* [in] */ unsigned int before,
            /* [in] */ IUIAutomationProxyFactoryEntry *factory);
        
        HRESULT ( __stdcall *RemoveEntry )( 
            IUIAutomationProxyFactoryMapping * This,
            /* [in] */ unsigned int index);
        
        HRESULT ( __stdcall *ClearTable )( 
            IUIAutomationProxyFactoryMapping * This);
        
        HRESULT ( __stdcall *RestoreDefaultTable )( 
            IUIAutomationProxyFactoryMapping * This);
        
        END_INTERFACE
    } IUIAutomationProxyFactoryMappingVtbl;

    interface IUIAutomationProxyFactoryMapping
    {
        CONST_VTBL struct IUIAutomationProxyFactoryMappingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomationProxyFactoryMapping_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomationProxyFactoryMapping_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomationProxyFactoryMapping_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomationProxyFactoryMapping_get_count(This,count)	\
    ( (This)->lpVtbl -> get_count(This,count) ) 

#define IUIAutomationProxyFactoryMapping_GetTable(This,table)	\
    ( (This)->lpVtbl -> GetTable(This,table) ) 

#define IUIAutomationProxyFactoryMapping_GetEntry(This,index,entry)	\
    ( (This)->lpVtbl -> GetEntry(This,index,entry) ) 

#define IUIAutomationProxyFactoryMapping_SetTable(This,factoryList)	\
    ( (This)->lpVtbl -> SetTable(This,factoryList) ) 

#define IUIAutomationProxyFactoryMapping_InsertEntries(This,before,factoryList)	\
    ( (This)->lpVtbl -> InsertEntries(This,before,factoryList) ) 

#define IUIAutomationProxyFactoryMapping_InsertEntry(This,before,factory)	\
    ( (This)->lpVtbl -> InsertEntry(This,before,factory) ) 

#define IUIAutomationProxyFactoryMapping_RemoveEntry(This,index)	\
    ( (This)->lpVtbl -> RemoveEntry(This,index) ) 

#define IUIAutomationProxyFactoryMapping_ClearTable(This)	\
    ( (This)->lpVtbl -> ClearTable(This) ) 

#define IUIAutomationProxyFactoryMapping_RestoreDefaultTable(This)	\
    ( (This)->lpVtbl -> RestoreDefaultTable(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomationProxyFactoryMapping_INTERFACE_DEFINED__ */


#ifndef __IUIAutomation_INTERFACE_DEFINED__
#define __IUIAutomation_INTERFACE_DEFINED__

/* interface IUIAutomation */
/* [object][uuid] */ 


EXTERN_C const IID IID_IUIAutomation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("30CBE57D-D9D0-452A-AB13-7AC5AC4825EE")
    IUIAutomation : public IUnknown
    {
    public:
        virtual HRESULT __stdcall CompareElements( 
            /* [in] */ IUIAutomationElement *el1,
            /* [in] */ IUIAutomationElement *el2,
            /* [retval][out] */ long *areSame) = 0;
        
        virtual HRESULT __stdcall CompareRuntimeIds( 
            /* [in] */ SAFEARRAY * runtimeId1,
            /* [in] */ SAFEARRAY * runtimeId2,
            /* [retval][out] */ long *areSame) = 0;
        
        virtual HRESULT __stdcall GetRootElement( 
            /* [retval][out] */ IUIAutomationElement **root) = 0;
        
        virtual HRESULT __stdcall ElementFromHandle( 
            /* [in] */ void *hwnd,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
        virtual HRESULT __stdcall ElementFromPoint( 
            /* [in] */ tagPOINT pt,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
        virtual HRESULT __stdcall GetFocusedElement( 
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
        virtual HRESULT __stdcall GetRootElementBuildCache( 
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **root) = 0;
        
        virtual HRESULT __stdcall ElementFromHandleBuildCache( 
            /* [in] */ void *hwnd,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
        virtual HRESULT __stdcall ElementFromPointBuildCache( 
            /* [in] */ tagPOINT pt,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
        virtual HRESULT __stdcall GetFocusedElementBuildCache( 
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
        virtual HRESULT __stdcall CreateTreeWalker( 
            /* [in] */ IUIAutomationCondition *pCondition,
            /* [retval][out] */ IUIAutomationTreeWalker **walker) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ControlViewWalker( 
            /* [retval][out] */ IUIAutomationTreeWalker **walker) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ContentViewWalker( 
            /* [retval][out] */ IUIAutomationTreeWalker **walker) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_RawViewWalker( 
            /* [retval][out] */ IUIAutomationTreeWalker **walker) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_RawViewCondition( 
            /* [retval][out] */ IUIAutomationCondition **condition) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ControlViewCondition( 
            /* [retval][out] */ IUIAutomationCondition **condition) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ContentViewCondition( 
            /* [retval][out] */ IUIAutomationCondition **condition) = 0;
        
        virtual HRESULT __stdcall CreateCacheRequest( 
            /* [retval][out] */ IUIAutomationCacheRequest **cacheRequest) = 0;
        
        virtual HRESULT __stdcall CreateTrueCondition( 
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreateFalseCondition( 
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreatePropertyCondition( 
            /* [in] */ int propertyId,
            /* [in] */ VARIANT value,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreatePropertyConditionEx( 
            /* [in] */ int propertyId,
            /* [in] */ VARIANT value,
            /* [in] */ PropertyConditionFlags flags,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreateAndCondition( 
            /* [in] */ IUIAutomationCondition *condition1,
            /* [in] */ IUIAutomationCondition *condition2,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreateAndConditionFromArray( 
            /* [in] */ SAFEARRAY * conditions,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreateAndConditionFromNativeArray( 
            /* [in] */ IUIAutomationCondition **conditions,
            /* [in] */ int conditionCount,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreateOrCondition( 
            /* [in] */ IUIAutomationCondition *condition1,
            /* [in] */ IUIAutomationCondition *condition2,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreateOrConditionFromArray( 
            /* [in] */ SAFEARRAY * conditions,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreateOrConditionFromNativeArray( 
            /* [in] */ IUIAutomationCondition **conditions,
            /* [in] */ int conditionCount,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall CreateNotCondition( 
            /* [in] */ IUIAutomationCondition *condition,
            /* [retval][out] */ IUIAutomationCondition **newCondition) = 0;
        
        virtual HRESULT __stdcall AddAutomationEventHandler( 
            /* [in] */ int eventId,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationEventHandler *handler) = 0;
        
        virtual HRESULT __stdcall RemoveAutomationEventHandler( 
            /* [in] */ int eventId,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationEventHandler *handler) = 0;
        
        virtual HRESULT __stdcall AddPropertyChangedEventHandlerNativeArray( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationPropertyChangedEventHandler *handler,
            /* [in] */ int *propertyArray,
            /* [in] */ int propertyCount) = 0;
        
        virtual HRESULT __stdcall AddPropertyChangedEventHandler( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationPropertyChangedEventHandler *handler,
            /* [in] */ SAFEARRAY * propertyArray) = 0;
        
        virtual HRESULT __stdcall RemovePropertyChangedEventHandler( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationPropertyChangedEventHandler *handler) = 0;
        
        virtual HRESULT __stdcall AddStructureChangedEventHandler( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationStructureChangedEventHandler *handler) = 0;
        
        virtual HRESULT __stdcall RemoveStructureChangedEventHandler( 
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationStructureChangedEventHandler *handler) = 0;
        
        virtual HRESULT __stdcall AddFocusChangedEventHandler( 
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationFocusChangedEventHandler *handler) = 0;
        
        virtual HRESULT __stdcall RemoveFocusChangedEventHandler( 
            /* [in] */ IUIAutomationFocusChangedEventHandler *handler) = 0;
        
        virtual HRESULT __stdcall RemoveAllEventHandlers( void) = 0;
        
        virtual HRESULT __stdcall IntNativeArrayToSafeArray( 
            /* [in] */ int *array,
            /* [in] */ int arrayCount,
            /* [retval][out] */ SAFEARRAY * *safeArray) = 0;
        
        virtual HRESULT __stdcall IntSafeArrayToNativeArray( 
            /* [in] */ SAFEARRAY * intArray,
            /* [out] */ int **array,
            /* [retval][out] */ int *arrayCount) = 0;
        
        virtual HRESULT __stdcall RectToVariant( 
            /* [in] */ tagRECT rc,
            /* [retval][out] */ VARIANT *var) = 0;
        
        virtual HRESULT __stdcall VariantToRect( 
            /* [in] */ VARIANT var,
            /* [retval][out] */ tagRECT *rc) = 0;
        
        virtual HRESULT __stdcall SafeArrayToRectNativeArray( 
            /* [in] */ SAFEARRAY * rects,
            /* [out] */ tagRECT **rectArray,
            /* [retval][out] */ int *rectArrayCount) = 0;
        
        virtual HRESULT __stdcall CreateProxyFactoryEntry( 
            /* [in] */ IUIAutomationProxyFactory *factory,
            /* [retval][out] */ IUIAutomationProxyFactoryEntry **factoryEntry) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ProxyFactoryMapping( 
            /* [retval][out] */ IUIAutomationProxyFactoryMapping **factoryMapping) = 0;
        
        virtual HRESULT __stdcall GetPropertyProgrammaticName( 
            /* [in] */ int property,
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual HRESULT __stdcall GetPatternProgrammaticName( 
            /* [in] */ int pattern,
            /* [retval][out] */ BSTR *name) = 0;
        
        virtual HRESULT __stdcall PollForPotentialSupportedPatterns( 
            /* [in] */ IUIAutomationElement *pElement,
            /* [out] */ SAFEARRAY * *patternIds,
            /* [out] */ SAFEARRAY * *patternNames) = 0;
        
        virtual HRESULT __stdcall PollForPotentialSupportedProperties( 
            /* [in] */ IUIAutomationElement *pElement,
            /* [out] */ SAFEARRAY * *propertyIds,
            /* [out] */ SAFEARRAY * *propertyNames) = 0;
        
        virtual HRESULT __stdcall CheckNotSupported( 
            /* [in] */ VARIANT value,
            /* [retval][out] */ long *isNotSupported) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ReservedNotSupportedValue( 
            /* [retval][out] */ IUnknown **notSupportedValue) = 0;
        
        virtual /* [propget] */ HRESULT __stdcall get_ReservedMixedAttributeValue( 
            /* [retval][out] */ IUnknown **mixedAttributeValue) = 0;
        
        virtual HRESULT __stdcall ElementFromIAccessible( 
            /* [in] */ IAccessible *accessible,
            /* [in] */ int childId,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
        virtual HRESULT __stdcall ElementFromIAccessibleBuildCache( 
            /* [in] */ IAccessible *accessible,
            /* [in] */ int childId,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **element) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IUIAutomationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUIAutomation * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUIAutomation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUIAutomation * This);
        
        HRESULT ( __stdcall *CompareElements )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationElement *el1,
            /* [in] */ IUIAutomationElement *el2,
            /* [retval][out] */ long *areSame);
        
        HRESULT ( __stdcall *CompareRuntimeIds )( 
            IUIAutomation * This,
            /* [in] */ SAFEARRAY * runtimeId1,
            /* [in] */ SAFEARRAY * runtimeId2,
            /* [retval][out] */ long *areSame);
        
        HRESULT ( __stdcall *GetRootElement )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationElement **root);
        
        HRESULT ( __stdcall *ElementFromHandle )( 
            IUIAutomation * This,
            /* [in] */ void *hwnd,
            /* [retval][out] */ IUIAutomationElement **element);
        
        HRESULT ( __stdcall *ElementFromPoint )( 
            IUIAutomation * This,
            /* [in] */ tagPOINT pt,
            /* [retval][out] */ IUIAutomationElement **element);
        
        HRESULT ( __stdcall *GetFocusedElement )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationElement **element);
        
        HRESULT ( __stdcall *GetRootElementBuildCache )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **root);
        
        HRESULT ( __stdcall *ElementFromHandleBuildCache )( 
            IUIAutomation * This,
            /* [in] */ void *hwnd,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **element);
        
        HRESULT ( __stdcall *ElementFromPointBuildCache )( 
            IUIAutomation * This,
            /* [in] */ tagPOINT pt,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **element);
        
        HRESULT ( __stdcall *GetFocusedElementBuildCache )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **element);
        
        HRESULT ( __stdcall *CreateTreeWalker )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCondition *pCondition,
            /* [retval][out] */ IUIAutomationTreeWalker **walker);
        
        /* [propget] */ HRESULT ( __stdcall *get_ControlViewWalker )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationTreeWalker **walker);
        
        /* [propget] */ HRESULT ( __stdcall *get_ContentViewWalker )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationTreeWalker **walker);
        
        /* [propget] */ HRESULT ( __stdcall *get_RawViewWalker )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationTreeWalker **walker);
        
        /* [propget] */ HRESULT ( __stdcall *get_RawViewCondition )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationCondition **condition);
        
        /* [propget] */ HRESULT ( __stdcall *get_ControlViewCondition )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationCondition **condition);
        
        /* [propget] */ HRESULT ( __stdcall *get_ContentViewCondition )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationCondition **condition);
        
        HRESULT ( __stdcall *CreateCacheRequest )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationCacheRequest **cacheRequest);
        
        HRESULT ( __stdcall *CreateTrueCondition )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreateFalseCondition )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreatePropertyCondition )( 
            IUIAutomation * This,
            /* [in] */ int propertyId,
            /* [in] */ VARIANT value,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreatePropertyConditionEx )( 
            IUIAutomation * This,
            /* [in] */ int propertyId,
            /* [in] */ VARIANT value,
            /* [in] */ PropertyConditionFlags flags,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreateAndCondition )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCondition *condition1,
            /* [in] */ IUIAutomationCondition *condition2,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreateAndConditionFromArray )( 
            IUIAutomation * This,
            /* [in] */ SAFEARRAY * conditions,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreateAndConditionFromNativeArray )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCondition **conditions,
            /* [in] */ int conditionCount,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreateOrCondition )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCondition *condition1,
            /* [in] */ IUIAutomationCondition *condition2,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreateOrConditionFromArray )( 
            IUIAutomation * This,
            /* [in] */ SAFEARRAY * conditions,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreateOrConditionFromNativeArray )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCondition **conditions,
            /* [in] */ int conditionCount,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *CreateNotCondition )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCondition *condition,
            /* [retval][out] */ IUIAutomationCondition **newCondition);
        
        HRESULT ( __stdcall *AddAutomationEventHandler )( 
            IUIAutomation * This,
            /* [in] */ int eventId,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationEventHandler *handler);
        
        HRESULT ( __stdcall *RemoveAutomationEventHandler )( 
            IUIAutomation * This,
            /* [in] */ int eventId,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationEventHandler *handler);
        
        HRESULT ( __stdcall *AddPropertyChangedEventHandlerNativeArray )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationPropertyChangedEventHandler *handler,
            /* [in] */ int *propertyArray,
            /* [in] */ int propertyCount);
        
        HRESULT ( __stdcall *AddPropertyChangedEventHandler )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationPropertyChangedEventHandler *handler,
            /* [in] */ SAFEARRAY * propertyArray);
        
        HRESULT ( __stdcall *RemovePropertyChangedEventHandler )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationPropertyChangedEventHandler *handler);
        
        HRESULT ( __stdcall *AddStructureChangedEventHandler )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ TreeScope scope,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationStructureChangedEventHandler *handler);
        
        HRESULT ( __stdcall *RemoveStructureChangedEventHandler )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationElement *element,
            /* [in] */ IUIAutomationStructureChangedEventHandler *handler);
        
        HRESULT ( __stdcall *AddFocusChangedEventHandler )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [in] */ IUIAutomationFocusChangedEventHandler *handler);
        
        HRESULT ( __stdcall *RemoveFocusChangedEventHandler )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationFocusChangedEventHandler *handler);
        
        HRESULT ( __stdcall *RemoveAllEventHandlers )( 
            IUIAutomation * This);
        
        HRESULT ( __stdcall *IntNativeArrayToSafeArray )( 
            IUIAutomation * This,
            /* [in] */ int *array,
            /* [in] */ int arrayCount,
            /* [retval][out] */ SAFEARRAY * *safeArray);
        
        HRESULT ( __stdcall *IntSafeArrayToNativeArray )( 
            IUIAutomation * This,
            /* [in] */ SAFEARRAY * intArray,
            /* [out] */ int **array,
            /* [retval][out] */ int *arrayCount);
        
        HRESULT ( __stdcall *RectToVariant )( 
            IUIAutomation * This,
            /* [in] */ tagRECT rc,
            /* [retval][out] */ VARIANT *var);
        
        HRESULT ( __stdcall *VariantToRect )( 
            IUIAutomation * This,
            /* [in] */ VARIANT var,
            /* [retval][out] */ tagRECT *rc);
        
        HRESULT ( __stdcall *SafeArrayToRectNativeArray )( 
            IUIAutomation * This,
            /* [in] */ SAFEARRAY * rects,
            /* [out] */ tagRECT **rectArray,
            /* [retval][out] */ int *rectArrayCount);
        
        HRESULT ( __stdcall *CreateProxyFactoryEntry )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationProxyFactory *factory,
            /* [retval][out] */ IUIAutomationProxyFactoryEntry **factoryEntry);
        
        /* [propget] */ HRESULT ( __stdcall *get_ProxyFactoryMapping )( 
            IUIAutomation * This,
            /* [retval][out] */ IUIAutomationProxyFactoryMapping **factoryMapping);
        
        HRESULT ( __stdcall *GetPropertyProgrammaticName )( 
            IUIAutomation * This,
            /* [in] */ int property,
            /* [retval][out] */ BSTR *name);
        
        HRESULT ( __stdcall *GetPatternProgrammaticName )( 
            IUIAutomation * This,
            /* [in] */ int pattern,
            /* [retval][out] */ BSTR *name);
        
        HRESULT ( __stdcall *PollForPotentialSupportedPatterns )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationElement *pElement,
            /* [out] */ SAFEARRAY * *patternIds,
            /* [out] */ SAFEARRAY * *patternNames);
        
        HRESULT ( __stdcall *PollForPotentialSupportedProperties )( 
            IUIAutomation * This,
            /* [in] */ IUIAutomationElement *pElement,
            /* [out] */ SAFEARRAY * *propertyIds,
            /* [out] */ SAFEARRAY * *propertyNames);
        
        HRESULT ( __stdcall *CheckNotSupported )( 
            IUIAutomation * This,
            /* [in] */ VARIANT value,
            /* [retval][out] */ long *isNotSupported);
        
        /* [propget] */ HRESULT ( __stdcall *get_ReservedNotSupportedValue )( 
            IUIAutomation * This,
            /* [retval][out] */ IUnknown **notSupportedValue);
        
        /* [propget] */ HRESULT ( __stdcall *get_ReservedMixedAttributeValue )( 
            IUIAutomation * This,
            /* [retval][out] */ IUnknown **mixedAttributeValue);
        
        HRESULT ( __stdcall *ElementFromIAccessible )( 
            IUIAutomation * This,
            /* [in] */ IAccessible *accessible,
            /* [in] */ int childId,
            /* [retval][out] */ IUIAutomationElement **element);
        
        HRESULT ( __stdcall *ElementFromIAccessibleBuildCache )( 
            IUIAutomation * This,
            /* [in] */ IAccessible *accessible,
            /* [in] */ int childId,
            /* [in] */ IUIAutomationCacheRequest *cacheRequest,
            /* [retval][out] */ IUIAutomationElement **element);
        
        END_INTERFACE
    } IUIAutomationVtbl;

    interface IUIAutomation
    {
        CONST_VTBL struct IUIAutomationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUIAutomation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IUIAutomation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IUIAutomation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IUIAutomation_CompareElements(This,el1,el2,areSame)	\
    ( (This)->lpVtbl -> CompareElements(This,el1,el2,areSame) ) 

#define IUIAutomation_CompareRuntimeIds(This,runtimeId1,runtimeId2,areSame)	\
    ( (This)->lpVtbl -> CompareRuntimeIds(This,runtimeId1,runtimeId2,areSame) ) 

#define IUIAutomation_GetRootElement(This,root)	\
    ( (This)->lpVtbl -> GetRootElement(This,root) ) 

#define IUIAutomation_ElementFromHandle(This,hwnd,element)	\
    ( (This)->lpVtbl -> ElementFromHandle(This,hwnd,element) ) 

#define IUIAutomation_ElementFromPoint(This,pt,element)	\
    ( (This)->lpVtbl -> ElementFromPoint(This,pt,element) ) 

#define IUIAutomation_GetFocusedElement(This,element)	\
    ( (This)->lpVtbl -> GetFocusedElement(This,element) ) 

#define IUIAutomation_GetRootElementBuildCache(This,cacheRequest,root)	\
    ( (This)->lpVtbl -> GetRootElementBuildCache(This,cacheRequest,root) ) 

#define IUIAutomation_ElementFromHandleBuildCache(This,hwnd,cacheRequest,element)	\
    ( (This)->lpVtbl -> ElementFromHandleBuildCache(This,hwnd,cacheRequest,element) ) 

#define IUIAutomation_ElementFromPointBuildCache(This,pt,cacheRequest,element)	\
    ( (This)->lpVtbl -> ElementFromPointBuildCache(This,pt,cacheRequest,element) ) 

#define IUIAutomation_GetFocusedElementBuildCache(This,cacheRequest,element)	\
    ( (This)->lpVtbl -> GetFocusedElementBuildCache(This,cacheRequest,element) ) 

#define IUIAutomation_CreateTreeWalker(This,pCondition,walker)	\
    ( (This)->lpVtbl -> CreateTreeWalker(This,pCondition,walker) ) 

#define IUIAutomation_get_ControlViewWalker(This,walker)	\
    ( (This)->lpVtbl -> get_ControlViewWalker(This,walker) ) 

#define IUIAutomation_get_ContentViewWalker(This,walker)	\
    ( (This)->lpVtbl -> get_ContentViewWalker(This,walker) ) 

#define IUIAutomation_get_RawViewWalker(This,walker)	\
    ( (This)->lpVtbl -> get_RawViewWalker(This,walker) ) 

#define IUIAutomation_get_RawViewCondition(This,condition)	\
    ( (This)->lpVtbl -> get_RawViewCondition(This,condition) ) 

#define IUIAutomation_get_ControlViewCondition(This,condition)	\
    ( (This)->lpVtbl -> get_ControlViewCondition(This,condition) ) 

#define IUIAutomation_get_ContentViewCondition(This,condition)	\
    ( (This)->lpVtbl -> get_ContentViewCondition(This,condition) ) 

#define IUIAutomation_CreateCacheRequest(This,cacheRequest)	\
    ( (This)->lpVtbl -> CreateCacheRequest(This,cacheRequest) ) 

#define IUIAutomation_CreateTrueCondition(This,newCondition)	\
    ( (This)->lpVtbl -> CreateTrueCondition(This,newCondition) ) 

#define IUIAutomation_CreateFalseCondition(This,newCondition)	\
    ( (This)->lpVtbl -> CreateFalseCondition(This,newCondition) ) 

#define IUIAutomation_CreatePropertyCondition(This,propertyId,value,newCondition)	\
    ( (This)->lpVtbl -> CreatePropertyCondition(This,propertyId,value,newCondition) ) 

#define IUIAutomation_CreatePropertyConditionEx(This,propertyId,value,flags,newCondition)	\
    ( (This)->lpVtbl -> CreatePropertyConditionEx(This,propertyId,value,flags,newCondition) ) 

#define IUIAutomation_CreateAndCondition(This,condition1,condition2,newCondition)	\
    ( (This)->lpVtbl -> CreateAndCondition(This,condition1,condition2,newCondition) ) 

#define IUIAutomation_CreateAndConditionFromArray(This,conditions,newCondition)	\
    ( (This)->lpVtbl -> CreateAndConditionFromArray(This,conditions,newCondition) ) 

#define IUIAutomation_CreateAndConditionFromNativeArray(This,conditions,conditionCount,newCondition)	\
    ( (This)->lpVtbl -> CreateAndConditionFromNativeArray(This,conditions,conditionCount,newCondition) ) 

#define IUIAutomation_CreateOrCondition(This,condition1,condition2,newCondition)	\
    ( (This)->lpVtbl -> CreateOrCondition(This,condition1,condition2,newCondition) ) 

#define IUIAutomation_CreateOrConditionFromArray(This,conditions,newCondition)	\
    ( (This)->lpVtbl -> CreateOrConditionFromArray(This,conditions,newCondition) ) 

#define IUIAutomation_CreateOrConditionFromNativeArray(This,conditions,conditionCount,newCondition)	\
    ( (This)->lpVtbl -> CreateOrConditionFromNativeArray(This,conditions,conditionCount,newCondition) ) 

#define IUIAutomation_CreateNotCondition(This,condition,newCondition)	\
    ( (This)->lpVtbl -> CreateNotCondition(This,condition,newCondition) ) 

#define IUIAutomation_AddAutomationEventHandler(This,eventId,element,scope,cacheRequest,handler)	\
    ( (This)->lpVtbl -> AddAutomationEventHandler(This,eventId,element,scope,cacheRequest,handler) ) 

#define IUIAutomation_RemoveAutomationEventHandler(This,eventId,element,handler)	\
    ( (This)->lpVtbl -> RemoveAutomationEventHandler(This,eventId,element,handler) ) 

#define IUIAutomation_AddPropertyChangedEventHandlerNativeArray(This,element,scope,cacheRequest,handler,propertyArray,propertyCount)	\
    ( (This)->lpVtbl -> AddPropertyChangedEventHandlerNativeArray(This,element,scope,cacheRequest,handler,propertyArray,propertyCount) ) 

#define IUIAutomation_AddPropertyChangedEventHandler(This,element,scope,cacheRequest,handler,propertyArray)	\
    ( (This)->lpVtbl -> AddPropertyChangedEventHandler(This,element,scope,cacheRequest,handler,propertyArray) ) 

#define IUIAutomation_RemovePropertyChangedEventHandler(This,element,handler)	\
    ( (This)->lpVtbl -> RemovePropertyChangedEventHandler(This,element,handler) ) 

#define IUIAutomation_AddStructureChangedEventHandler(This,element,scope,cacheRequest,handler)	\
    ( (This)->lpVtbl -> AddStructureChangedEventHandler(This,element,scope,cacheRequest,handler) ) 

#define IUIAutomation_RemoveStructureChangedEventHandler(This,element,handler)	\
    ( (This)->lpVtbl -> RemoveStructureChangedEventHandler(This,element,handler) ) 

#define IUIAutomation_AddFocusChangedEventHandler(This,cacheRequest,handler)	\
    ( (This)->lpVtbl -> AddFocusChangedEventHandler(This,cacheRequest,handler) ) 

#define IUIAutomation_RemoveFocusChangedEventHandler(This,handler)	\
    ( (This)->lpVtbl -> RemoveFocusChangedEventHandler(This,handler) ) 

#define IUIAutomation_RemoveAllEventHandlers(This)	\
    ( (This)->lpVtbl -> RemoveAllEventHandlers(This) ) 

#define IUIAutomation_IntNativeArrayToSafeArray(This,array,arrayCount,safeArray)	\
    ( (This)->lpVtbl -> IntNativeArrayToSafeArray(This,array,arrayCount,safeArray) ) 

#define IUIAutomation_IntSafeArrayToNativeArray(This,intArray,array,arrayCount)	\
    ( (This)->lpVtbl -> IntSafeArrayToNativeArray(This,intArray,array,arrayCount) ) 

#define IUIAutomation_RectToVariant(This,rc,var)	\
    ( (This)->lpVtbl -> RectToVariant(This,rc,var) ) 

#define IUIAutomation_VariantToRect(This,var,rc)	\
    ( (This)->lpVtbl -> VariantToRect(This,var,rc) ) 

#define IUIAutomation_SafeArrayToRectNativeArray(This,rects,rectArray,rectArrayCount)	\
    ( (This)->lpVtbl -> SafeArrayToRectNativeArray(This,rects,rectArray,rectArrayCount) ) 

#define IUIAutomation_CreateProxyFactoryEntry(This,factory,factoryEntry)	\
    ( (This)->lpVtbl -> CreateProxyFactoryEntry(This,factory,factoryEntry) ) 

#define IUIAutomation_get_ProxyFactoryMapping(This,factoryMapping)	\
    ( (This)->lpVtbl -> get_ProxyFactoryMapping(This,factoryMapping) ) 

#define IUIAutomation_GetPropertyProgrammaticName(This,property,name)	\
    ( (This)->lpVtbl -> GetPropertyProgrammaticName(This,property,name) ) 

#define IUIAutomation_GetPatternProgrammaticName(This,pattern,name)	\
    ( (This)->lpVtbl -> GetPatternProgrammaticName(This,pattern,name) ) 

#define IUIAutomation_PollForPotentialSupportedPatterns(This,pElement,patternIds,patternNames)	\
    ( (This)->lpVtbl -> PollForPotentialSupportedPatterns(This,pElement,patternIds,patternNames) ) 

#define IUIAutomation_PollForPotentialSupportedProperties(This,pElement,propertyIds,propertyNames)	\
    ( (This)->lpVtbl -> PollForPotentialSupportedProperties(This,pElement,propertyIds,propertyNames) ) 

#define IUIAutomation_CheckNotSupported(This,value,isNotSupported)	\
    ( (This)->lpVtbl -> CheckNotSupported(This,value,isNotSupported) ) 

#define IUIAutomation_get_ReservedNotSupportedValue(This,notSupportedValue)	\
    ( (This)->lpVtbl -> get_ReservedNotSupportedValue(This,notSupportedValue) ) 

#define IUIAutomation_get_ReservedMixedAttributeValue(This,mixedAttributeValue)	\
    ( (This)->lpVtbl -> get_ReservedMixedAttributeValue(This,mixedAttributeValue) ) 

#define IUIAutomation_ElementFromIAccessible(This,accessible,childId,element)	\
    ( (This)->lpVtbl -> ElementFromIAccessible(This,accessible,childId,element) ) 

#define IUIAutomation_ElementFromIAccessibleBuildCache(This,accessible,childId,cacheRequest,element)	\
    ( (This)->lpVtbl -> ElementFromIAccessibleBuildCache(This,accessible,childId,cacheRequest,element) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IUIAutomation_INTERFACE_DEFINED__ */



#ifndef __UIA_PatternIds_MODULE_DEFINED__
#define __UIA_PatternIds_MODULE_DEFINED__


/* module UIA_PatternIds */
/* [dllname] */ 

const long UIA_InvokePatternId	=	10000;

const long UIA_SelectionPatternId	=	10001;

const long UIA_ValuePatternId	=	10002;

const long UIA_RangeValuePatternId	=	10003;

const long UIA_ScrollPatternId	=	10004;

const long UIA_ExpandCollapsePatternId	=	10005;

const long UIA_GridPatternId	=	10006;

const long UIA_GridItemPatternId	=	10007;

const long UIA_MultipleViewPatternId	=	10008;

const long UIA_WindowPatternId	=	10009;

const long UIA_SelectionItemPatternId	=	10010;

const long UIA_DockPatternId	=	10011;

const long UIA_TablePatternId	=	10012;

const long UIA_TableItemPatternId	=	10013;

const long UIA_TextPatternId	=	10014;

const long UIA_TogglePatternId	=	10015;

const long UIA_TransformPatternId	=	10016;

const long UIA_ScrollItemPatternId	=	10017;

const long UIA_LegacyIAccessiblePatternId	=	10018;

const long UIA_ItemContainerPatternId	=	10019;

const long UIA_VirtualizedItemPatternId	=	10020;

const long UIA_SynchronizedInputPatternId	=	10021;

#endif /* __UIA_PatternIds_MODULE_DEFINED__ */


#ifndef __UIA_EventIds_MODULE_DEFINED__
#define __UIA_EventIds_MODULE_DEFINED__


/* module UIA_EventIds */
/* [dllname] */ 

const long UIA_ToolTipOpenedEventId	=	20000;

const long UIA_ToolTipClosedEventId	=	20001;

const long UIA_StructureChangedEventId	=	20002;

const long UIA_MenuOpenedEventId	=	20003;

const long UIA_AutomationPropertyChangedEventId	=	20004;

const long UIA_AutomationFocusChangedEventId	=	20005;

const long UIA_AsyncContentLoadedEventId	=	20006;

const long UIA_MenuClosedEventId	=	20007;

const long UIA_LayoutInvalidatedEventId	=	20008;

const long UIA_Invoke_InvokedEventId	=	20009;

const long UIA_SelectionItem_ElementAddedToSelectionEventId	=	20010;

const long UIA_SelectionItem_ElementRemovedFromSelectionEventId	=	20011;

const long UIA_SelectionItem_ElementSelectedEventId	=	20012;

const long UIA_Selection_InvalidatedEventId	=	20013;

const long UIA_Text_TextSelectionChangedEventId	=	20014;

const long UIA_Text_TextChangedEventId	=	20015;

const long UIA_Window_WindowOpenedEventId	=	20016;

const long UIA_Window_WindowClosedEventId	=	20017;

const long UIA_MenuModeStartEventId	=	20018;

const long UIA_MenuModeEndEventId	=	20019;

const long UIA_InputReachedTargetEventId	=	20020;

const long UIA_InputReachedOtherElementEventId	=	20021;

const long UIA_InputDiscardedEventId	=	20022;

#endif /* __UIA_EventIds_MODULE_DEFINED__ */


#ifndef __UIA_PropertyIds_MODULE_DEFINED__
#define __UIA_PropertyIds_MODULE_DEFINED__


/* module UIA_PropertyIds */
/* [dllname] */ 

const long UIA_RuntimeIdPropertyId	=	30000;

const long UIA_BoundingRectanglePropertyId	=	30001;

const long UIA_ProcessIdPropertyId	=	30002;

const long UIA_ControlTypePropertyId	=	30003;

const long UIA_LocalizedControlTypePropertyId	=	30004;

const long UIA_NamePropertyId	=	30005;

const long UIA_AcceleratorKeyPropertyId	=	30006;

const long UIA_AccessKeyPropertyId	=	30007;

const long UIA_HasKeyboardFocusPropertyId	=	30008;

const long UIA_IsKeyboardFocusablePropertyId	=	30009;

const long UIA_IsEnabledPropertyId	=	30010;

const long UIA_AutomationIdPropertyId	=	30011;

const long UIA_ClassNamePropertyId	=	30012;

const long UIA_HelpTextPropertyId	=	30013;

const long UIA_ClickablePointPropertyId	=	30014;

const long UIA_CulturePropertyId	=	30015;

const long UIA_IsControlElementPropertyId	=	30016;

const long UIA_IsContentElementPropertyId	=	30017;

const long UIA_LabeledByPropertyId	=	30018;

const long UIA_IsPasswordPropertyId	=	30019;

const long UIA_NativeWindowHandlePropertyId	=	30020;

const long UIA_ItemTypePropertyId	=	30021;

const long UIA_IsOffscreenPropertyId	=	30022;

const long UIA_OrientationPropertyId	=	30023;

const long UIA_FrameworkIdPropertyId	=	30024;

const long UIA_IsRequiredForFormPropertyId	=	30025;

const long UIA_ItemStatusPropertyId	=	30026;

const long UIA_IsDockPatternAvailablePropertyId	=	30027;

const long UIA_IsExpandCollapsePatternAvailablePropertyId	=	30028;

const long UIA_IsGridItemPatternAvailablePropertyId	=	30029;

const long UIA_IsGridPatternAvailablePropertyId	=	30030;

const long UIA_IsInvokePatternAvailablePropertyId	=	30031;

const long UIA_IsMultipleViewPatternAvailablePropertyId	=	30032;

const long UIA_IsRangeValuePatternAvailablePropertyId	=	30033;

const long UIA_IsScrollPatternAvailablePropertyId	=	30034;

const long UIA_IsScrollItemPatternAvailablePropertyId	=	30035;

const long UIA_IsSelectionItemPatternAvailablePropertyId	=	30036;

const long UIA_IsSelectionPatternAvailablePropertyId	=	30037;

const long UIA_IsTablePatternAvailablePropertyId	=	30038;

const long UIA_IsTableItemPatternAvailablePropertyId	=	30039;

const long UIA_IsTextPatternAvailablePropertyId	=	30040;

const long UIA_IsTogglePatternAvailablePropertyId	=	30041;

const long UIA_IsTransformPatternAvailablePropertyId	=	30042;

const long UIA_IsValuePatternAvailablePropertyId	=	30043;

const long UIA_IsWindowPatternAvailablePropertyId	=	30044;

const long UIA_ValueValuePropertyId	=	30045;

const long UIA_ValueIsReadOnlyPropertyId	=	30046;

const long UIA_RangeValueValuePropertyId	=	30047;

const long UIA_RangeValueIsReadOnlyPropertyId	=	30048;

const long UIA_RangeValueMinimumPropertyId	=	30049;

const long UIA_RangeValueMaximumPropertyId	=	30050;

const long UIA_RangeValueLargeChangePropertyId	=	30051;

const long UIA_RangeValueSmallChangePropertyId	=	30052;

const long UIA_ScrollHorizontalScrollPercentPropertyId	=	30053;

const long UIA_ScrollHorizontalViewSizePropertyId	=	30054;

const long UIA_ScrollVerticalScrollPercentPropertyId	=	30055;

const long UIA_ScrollVerticalViewSizePropertyId	=	30056;

const long UIA_ScrollHorizontallyScrollablePropertyId	=	30057;

const long UIA_ScrollVerticallyScrollablePropertyId	=	30058;

const long UIA_SelectionSelectionPropertyId	=	30059;

const long UIA_SelectionCanSelectMultiplePropertyId	=	30060;

const long UIA_SelectionIsSelectionRequiredPropertyId	=	30061;

const long UIA_GridRowCountPropertyId	=	30062;

const long UIA_GridColumnCountPropertyId	=	30063;

const long UIA_GridItemRowPropertyId	=	30064;

const long UIA_GridItemColumnPropertyId	=	30065;

const long UIA_GridItemRowSpanPropertyId	=	30066;

const long UIA_GridItemColumnSpanPropertyId	=	30067;

const long UIA_GridItemContainingGridPropertyId	=	30068;

const long UIA_DockDockPositionPropertyId	=	30069;

const long UIA_ExpandCollapseExpandCollapseStatePropertyId	=	30070;

const long UIA_MultipleViewCurrentViewPropertyId	=	30071;

const long UIA_MultipleViewSupportedViewsPropertyId	=	30072;

const long UIA_WindowCanMaximizePropertyId	=	30073;

const long UIA_WindowCanMinimizePropertyId	=	30074;

const long UIA_WindowWindowVisualStatePropertyId	=	30075;

const long UIA_WindowWindowInteractionStatePropertyId	=	30076;

const long UIA_WindowIsModalPropertyId	=	30077;

const long UIA_WindowIsTopmostPropertyId	=	30078;

const long UIA_SelectionItemIsSelectedPropertyId	=	30079;

const long UIA_SelectionItemSelectionContainerPropertyId	=	30080;

const long UIA_TableRowHeadersPropertyId	=	30081;

const long UIA_TableColumnHeadersPropertyId	=	30082;

const long UIA_TableRowOrColumnMajorPropertyId	=	30083;

const long UIA_TableItemRowHeaderItemsPropertyId	=	30084;

const long UIA_TableItemColumnHeaderItemsPropertyId	=	30085;

const long UIA_ToggleToggleStatePropertyId	=	30086;

const long UIA_TransformCanMovePropertyId	=	30087;

const long UIA_TransformCanResizePropertyId	=	30088;

const long UIA_TransformCanRotatePropertyId	=	30089;

const long UIA_IsLegacyIAccessiblePatternAvailablePropertyId	=	30090;

const long UIA_LegacyIAccessibleChildIdPropertyId	=	30091;

const long UIA_LegacyIAccessibleNamePropertyId	=	30092;

const long UIA_LegacyIAccessibleValuePropertyId	=	30093;

const long UIA_LegacyIAccessibleDescriptionPropertyId	=	30094;

const long UIA_LegacyIAccessibleRolePropertyId	=	30095;

const long UIA_LegacyIAccessibleStatePropertyId	=	30096;

const long UIA_LegacyIAccessibleHelpPropertyId	=	30097;

const long UIA_LegacyIAccessibleKeyboardShortcutPropertyId	=	30098;

const long UIA_LegacyIAccessibleSelectionPropertyId	=	30099;

const long UIA_LegacyIAccessibleDefaultActionPropertyId	=	30100;

const long UIA_AriaRolePropertyId	=	30101;

const long UIA_AriaPropertiesPropertyId	=	30102;

const long UIA_IsDataValidForFormPropertyId	=	30103;

const long UIA_ControllerForPropertyId	=	30104;

const long UIA_DescribedByPropertyId	=	30105;

const long UIA_FlowsToPropertyId	=	30106;

const long UIA_ProviderDescriptionPropertyId	=	30107;

const long UIA_IsItemContainerPatternAvailablePropertyId	=	30108;

const long UIA_IsVirtualizedItemPatternAvailablePropertyId	=	30109;

const long UIA_IsSynchronizedInputPatternAvailablePropertyId	=	30110;

#endif /* __UIA_PropertyIds_MODULE_DEFINED__ */


#ifndef __UIA_TextAttributeIds_MODULE_DEFINED__
#define __UIA_TextAttributeIds_MODULE_DEFINED__


/* module UIA_TextAttributeIds */
/* [dllname] */ 

const long UIA_AnimationStyleAttributeId	=	40000;

const long UIA_BackgroundColorAttributeId	=	40001;

const long UIA_BulletStyleAttributeId	=	40002;

const long UIA_CapStyleAttributeId	=	40003;

const long UIA_CultureAttributeId	=	40004;

const long UIA_FontNameAttributeId	=	40005;

const long UIA_FontSizeAttributeId	=	40006;

const long UIA_FontWeightAttributeId	=	40007;

const long UIA_ForegroundColorAttributeId	=	40008;

const long UIA_HorizontalTextAlignmentAttributeId	=	40009;

const long UIA_IndentationFirstLineAttributeId	=	40010;

const long UIA_IndentationLeadingAttributeId	=	40011;

const long UIA_IndentationTrailingAttributeId	=	40012;

const long UIA_IsHiddenAttributeId	=	40013;

const long UIA_IsItalicAttributeId	=	40014;

const long UIA_IsReadOnlyAttributeId	=	40015;

const long UIA_IsSubscriptAttributeId	=	40016;

const long UIA_IsSuperscriptAttributeId	=	40017;

const long UIA_MarginBottomAttributeId	=	40018;

const long UIA_MarginLeadingAttributeId	=	40019;

const long UIA_MarginTopAttributeId	=	40020;

const long UIA_MarginTrailingAttributeId	=	40021;

const long UIA_OutlineStylesAttributeId	=	40022;

const long UIA_OverlineColorAttributeId	=	40023;

const long UIA_OverlineStyleAttributeId	=	40024;

const long UIA_StrikethroughColorAttributeId	=	40025;

const long UIA_StrikethroughStyleAttributeId	=	40026;

const long UIA_TabsAttributeId	=	40027;

const long UIA_TextFlowDirectionsAttributeId	=	40028;

const long UIA_UnderlineColorAttributeId	=	40029;

const long UIA_UnderlineStyleAttributeId	=	40030;

#endif /* __UIA_TextAttributeIds_MODULE_DEFINED__ */


#ifndef __UIA_ControlTypeIds_MODULE_DEFINED__
#define __UIA_ControlTypeIds_MODULE_DEFINED__


/* module UIA_ControlTypeIds */
/* [dllname] */ 

const long UIA_ButtonControlTypeId	=	50000;

const long UIA_CalendarControlTypeId	=	50001;

const long UIA_CheckBoxControlTypeId	=	50002;

const long UIA_ComboBoxControlTypeId	=	50003;

const long UIA_EditControlTypeId	=	50004;

const long UIA_HyperlinkControlTypeId	=	50005;

const long UIA_ImageControlTypeId	=	50006;

const long UIA_ListItemControlTypeId	=	50007;

const long UIA_ListControlTypeId	=	50008;

const long UIA_MenuControlTypeId	=	50009;

const long UIA_MenuBarControlTypeId	=	50010;

const long UIA_MenuItemControlTypeId	=	50011;

const long UIA_ProgressBarControlTypeId	=	50012;

const long UIA_RadioButtonControlTypeId	=	50013;

const long UIA_ScrollBarControlTypeId	=	50014;

const long UIA_SliderControlTypeId	=	50015;

const long UIA_SpinnerControlTypeId	=	50016;

const long UIA_StatusBarControlTypeId	=	50017;

const long UIA_TabControlTypeId	=	50018;

const long UIA_TabItemControlTypeId	=	50019;

const long UIA_TextControlTypeId	=	50020;

const long UIA_ToolBarControlTypeId	=	50021;

const long UIA_ToolTipControlTypeId	=	50022;

const long UIA_TreeControlTypeId	=	50023;

const long UIA_TreeItemControlTypeId	=	50024;

const long UIA_CustomControlTypeId	=	50025;

const long UIA_GroupControlTypeId	=	50026;

const long UIA_ThumbControlTypeId	=	50027;

const long UIA_DataGridControlTypeId	=	50028;

const long UIA_DataItemControlTypeId	=	50029;

const long UIA_DocumentControlTypeId	=	50030;

const long UIA_SplitButtonControlTypeId	=	50031;

const long UIA_WindowControlTypeId	=	50032;

const long UIA_PaneControlTypeId	=	50033;

const long UIA_HeaderControlTypeId	=	50034;

const long UIA_HeaderItemControlTypeId	=	50035;

const long UIA_TableControlTypeId	=	50036;

const long UIA_TitleBarControlTypeId	=	50037;

const long UIA_SeparatorControlTypeId	=	50038;

#endif /* __UIA_ControlTypeIds_MODULE_DEFINED__ */

EXTERN_C const CLSID CLSID_CUIAutomation;

#ifdef __cplusplus

class DECLSPEC_UUID("FF48DBA4-60EF-4201-AA87-54103EEF594E")
CUIAutomation;
#endif
#endif /* __UIAutomationClient_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type, name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

    typedef struct _IID
    {
        unsigned long x;
        unsigned short s1;
        unsigned short s2;
        unsigned char c[8];
    } IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
    typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type, name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    __declspec(selectany) const type name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IUIAutomation,            0x30cbe57d, 0xd9d0, 0x452a, 0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee);
MIDL_DEFINE_GUID(CLSID, CLSID_CUIAutomation,        0xff48dba4, 0x60ef, 0x4201, 0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e);
MIDL_DEFINE_GUID(IID, IID_IUIAutomationElement,     0xd22108aa, 0x8ac5, 0x49a5, 0x83, 0x7b, 0x37, 0xbb, 0xb3, 0xd7, 0x59, 0x1e);
MIDL_DEFINE_GUID(IID, IID_IUIAutomationTextPattern, 0x32eba289, 0x3583, 0x42c9, 0x9c, 0x59, 0x3b, 0x6d, 0x9a, 0x1e, 0x9b, 0x6a);
MIDL_DEFINE_GUID(IID, IID_IUIAutomationTextRange,   0xa543cc6a, 0xf4ae, 0x494b, 0x82, 0x39, 0xc8, 0x14, 0x48, 0x11, 0x87, 0xa8);
MIDL_DEFINE_GUID(IID, IID_IUIAutomationTreeWalker,  0x4042c624, 0x389c, 0x4afc, 0xa6, 0x30, 0x9d, 0xf8, 0x54, 0xa5, 0x41, 0xfc);
#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif

#endif


