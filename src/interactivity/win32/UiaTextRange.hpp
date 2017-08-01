/*++
Copyright (c) Microsoft Corporation

Module Name:
- UiaTextRange.hpp

Abstract:
- This module provides UI Automation access to the text of the console
  window to support both automation tests and accessibility (screen
  reading) applications.

Author(s):
- Austin Diviness (AustDi)     2017
--*/

#pragma once

#include "precomp.h"

#include "../inc/IConsoleWindow.hpp"
#include "../../host/cursor.h"

#include <deque>

#ifdef UNIT_TESTING
class UiaTextRangeTests;
#endif


// The UiaTextRange deals with several data structures that have
// similar semantics. In order to keep the information from these data
// structures separated, each structure has its own naming for a
// row.
//
// There is the generic Row, which does not know which data structure
// the row came from.
//
// There is the ViewportRow, which is a 0-indexed row value from the
// viewport. The top row of the viewport is at 0, rows below the top
// row increase in value and rows above the top row get increasingly
// negative.
//
// ScreenInfoRow is a row from the screen info data structure. They
// start at 0 at the top of screen info buffer. Their positions do not
// change but their associated row in the text buffer does change each
// time a new line is written.
//
// TextBufferRow is a row from the text buffer. It is not a ROW
// struct, but rather the index of a row. This is also 0-indexed. A
// TextBufferRow with a value of 0 does not necessarily refer to the
// top row of the console.

typedef int Row;
typedef int ViewportRow;
typedef unsigned int ScreenInfoRow;
typedef unsigned int TextBufferRow;

typedef SMALL_RECT Viewport;
typedef unsigned long long IdType;

// A Column is a row agnostic value that refers to the column an
// endpoint is equivalent to. It is 0-indexed.
typedef unsigned int Column;

// an endpoint is a char location in the text buffer. endpoint 0 is
// the first char of the 0th row in the text buffer row array.
typedef unsigned int Endpoint;

namespace Microsoft
{
    namespace Console
    {
        namespace Interactivity
        {
            namespace Win32
            {

                class UiaTextRange final : public ITextRangeProvider
                {
                private:
                    static IdType id;

                public:

                    static std::deque<UiaTextRange*> GetSelectionRanges(_In_ IRawElementProviderSimple* pProvider);

                    // degenerate range
                    static UiaTextRange* Create(_In_ IRawElementProviderSimple* const pProvider);

                    // degenerate range at cursor position
                    static UiaTextRange* Create(_In_ IRawElementProviderSimple* const pProvider,
                                                _In_ const Cursor* const pCursor);

                    // specific endpoint range
                    static UiaTextRange* Create(_In_ IRawElementProviderSimple* const pProvider,
                                                _In_ const Endpoint start,
                                                _In_ const Endpoint end,
                                                _In_ const bool degenerate);

                    // range from a UiaPoint
                    static UiaTextRange* Create(_In_ IRawElementProviderSimple* const pProvider,
                                                _In_ const UiaPoint point);

                    ~UiaTextRange();


                    const IdType GetId() const;
                    const Endpoint GetStart() const;
                    const Endpoint GetEnd() const;
                    const bool IsDegenerate() const;

                    // IUnknown methods
                    IFACEMETHODIMP_(ULONG) AddRef();
                    IFACEMETHODIMP_(ULONG) Release();
                    IFACEMETHODIMP QueryInterface(_In_ REFIID riid,
                                                  _COM_Outptr_result_maybenull_ void** ppInterface);

                    // ITextRangeProvider methods
                    IFACEMETHODIMP Clone(_Outptr_result_maybenull_ ITextRangeProvider** ppRetVal);
                    IFACEMETHODIMP Compare(_In_opt_ ITextRangeProvider* pRange, _Out_ BOOL* pRetVal);
                    IFACEMETHODIMP CompareEndpoints(_In_ TextPatternRangeEndpoint endpoint,
                                                    _In_ ITextRangeProvider* pTargetRange,
                                                    _In_ TextPatternRangeEndpoint targetEndpoint,
                                                    _Out_ int* pRetVal);
                    IFACEMETHODIMP ExpandToEnclosingUnit(_In_ TextUnit unit);
                    IFACEMETHODIMP FindAttribute(_In_ TEXTATTRIBUTEID textAttributeId,
                                                 _In_ VARIANT val,
                                                 _In_ BOOL searchBackward,
                                                 _Outptr_result_maybenull_ ITextRangeProvider** ppRetVal);
                    IFACEMETHODIMP FindText(_In_ BSTR text,
                                            _In_ BOOL searchBackward,
                                            _In_ BOOL ignoreCase,
                                            _Outptr_result_maybenull_ ITextRangeProvider** ppRetVal);
                    IFACEMETHODIMP GetAttributeValue(_In_ TEXTATTRIBUTEID textAttributeId,
                                                     _Out_ VARIANT* pRetVal);
                    IFACEMETHODIMP GetBoundingRectangles(_Outptr_result_maybenull_ SAFEARRAY** ppRetVal);
                    IFACEMETHODIMP GetEnclosingElement(_Outptr_result_maybenull_ IRawElementProviderSimple** ppRetVal);
                    IFACEMETHODIMP GetText(_In_ int maxLength,
                                           _Out_ BSTR* pRetVal);
                    IFACEMETHODIMP Move(_In_ TextUnit unit,
                                        _In_ int count,
                                        _Out_ int* pRetVal);
                    IFACEMETHODIMP MoveEndpointByUnit(_In_ TextPatternRangeEndpoint endpoint,
                                                      _In_ TextUnit unit,
                                                      _In_ int count,
                                                      _Out_ int* pRetVal);
                    IFACEMETHODIMP MoveEndpointByRange(_In_ TextPatternRangeEndpoint endpoint,
                                                       _In_ ITextRangeProvider* pTargetRange,
                                                       _In_ TextPatternRangeEndpoint targetEndpoint);
                    IFACEMETHODIMP Select();
                    IFACEMETHODIMP AddToSelection();
                    IFACEMETHODIMP RemoveFromSelection();
                    IFACEMETHODIMP ScrollIntoView(_In_ BOOL alignToTop);
                    IFACEMETHODIMP GetChildren(_Outptr_result_maybenull_ SAFEARRAY** ppRetVal);

                protected:
#if _DEBUG
                    void _outputRowConversions();
                    void _outputObjectState();
#endif

                    IRawElementProviderSimple* const _pProvider;

                private:
                    // degenerate range
                    UiaTextRange(_In_ IRawElementProviderSimple* const pProvider);

                    // degenerate range at cursor position
                    UiaTextRange(_In_ IRawElementProviderSimple* const pProvider,
                                 _In_ const Cursor* const pCursor);

                    // specific endpoint range
                    UiaTextRange(_In_ IRawElementProviderSimple* const pProvider,
                                 _In_ const Endpoint start,
                                 _In_ const Endpoint end,
                                 _In_ const bool degenerate);

                    // range from a UiaPoint
                    UiaTextRange(_In_ IRawElementProviderSimple* const pProvider,
                                 _In_ const UiaPoint point);

                    UiaTextRange(_In_ const UiaTextRange& a);

                    // used to debug objects passed back and forth
                    // between the provider and the client
                    IdType _id;

                    // Ref counter for COM object
                    ULONG _cRefs;

                    // measure units in the form [_start, _end]. _start
                    // may be a bigger number than _end if the range
                    // wraps around the end of the text buffer.
                    //
                    // In this scenario, _start <= _end
                    // 0 ............... N (text buffer line indices)
                    //      s-----e        (_start to _end)
                    //
                    // In this scenario, _start >= end
                    // 0 ............... N (text buffer line indices)
                    //   ---e     s-----   (_start to _end)
                    //
                    Endpoint _start;
                    Endpoint _end;

                    // The msdn documentation (and hence this class) talks a bunch about a
                    // degenerate range. A range is degenerate if it contains
                    // no text (both the start and end endpoints are the same). Note that
                    // a degenerate range may have a position in the text. We indicate a
                    // degenerate range internally with a bool. If a range is degenerate
                    // then both endpoints will contain the same value.
                    bool _degenerate;

                    static const Viewport _getViewport();
                    static HWND _getWindowHandle();
                    static IConsoleWindow* const _getIConsoleWindow();
                    static SCREEN_INFORMATION* const _getScreenInfo();
                    static TEXT_BUFFER_INFO* const _getTextBuffer();
                    static const COORD _getScreenBufferCoords();

                    static const unsigned int _getTotalRows();
                    static const unsigned int _getRowWidth();

                    static const unsigned int _getFirstScreenInfoRowIndex();
                    static const unsigned int _getLastScreenInfoRowIndex();

                    static const Column _getFirstColumnIndex();
                    static const Column _getLastColumnIndex();

                    const unsigned int _rowCountInRange() const;

                    static const TextBufferRow _endpointToTextBufferRow(_In_ const Endpoint endpoint);
                    static const ScreenInfoRow _textBufferRowToScreenInfoRow(_In_ const TextBufferRow row);

                    static const TextBufferRow _screenInfoRowToTextBufferRow(_In_ const ScreenInfoRow row);
                    static const Endpoint _textBufferRowToEndpoint(_In_ const TextBufferRow row);

                    static const ScreenInfoRow _endpointToScreenInfoRow(_In_ const Endpoint endpoint);
                    static const Endpoint _screenInfoRowToEndpoint(_In_ const ScreenInfoRow row);

                    static const Column _endpointToColumn(_In_ const Endpoint endpoint);

                    static const Row _normalizeRow(_In_ const Row row);

                    static const ViewportRow _screenInfoRowToViewportRow(_In_ const ScreenInfoRow row);
                    static const ViewportRow _screenInfoRowToViewportRow(_In_ const ScreenInfoRow row,
                                                                         _In_ const Viewport viewport);

                    static const bool _isScreenInfoRowInViewport(_In_ const ScreenInfoRow row);
                    static const bool _isScreenInfoRowInViewport(_In_ const ScreenInfoRow row,
                                                                 _In_ const Viewport viewport);

                    static const unsigned int _getViewportHeight(_In_ const Viewport viewport);
                    static const unsigned int _getViewportWidth(_In_ const Viewport viewport);

                    void _addScreenInfoRowBoundaries(_In_ const ScreenInfoRow screenInfoRow,
                                                     _Inout_ std::vector<double>& coords) const;

                    static const int _compareScreenCoords(_In_ const ScreenInfoRow rowA,
                                                          _In_ const Column colA,
                                                          _In_ const ScreenInfoRow rowB,
                                                          _In_ const Column colB);

#ifdef UNIT_TESTING
                    friend class ::UiaTextRangeTests;
#endif
                };

                namespace UiaTextRangeTracing
                {

                    enum class ApiCall
                    {
                        Constructor,
                        AddRef,
                        Release,
                        QueryInterface,
                        Clone,
                        Compare,
                        CompareEndpoints,
                        ExpandToEnclosingUnit,
                        FindAttribute,
                        FindText,
                        GetAttributeValue,
                        GetBoundingRectangles,
                        GetEnclosingElement,
                        GetText,
                        Move,
                        MoveEndpointByUnit,
                        MoveEndpointByRange,
                        Select,
                        AddToSelection,
                        RemoveFromSelection,
                        ScrollIntoView,
                        GetChildren
                    };

                    struct IApiMsg
                    {
                    };

                    struct ApiMsgConstructor : public IApiMsg
                    {
                        IdType Id;
                    };

                    struct ApiMsgClone : public IApiMsg
                    {
                        IdType CloneId;
                    };

                    struct ApiMsgCompare : public IApiMsg
                    {
                        IdType OtherId;
                        bool Equal;
                    };

                    struct ApiMsgCompareEndpoints : public IApiMsg
                    {
                        IdType OtherId;
                        TextPatternRangeEndpoint Endpoint;
                        TextPatternRangeEndpoint TargetEndpoint;
                        int Result;
                    };

                    struct ApiMsgExpandToEnclosingUnit : public IApiMsg
                    {
                        TextUnit Unit;
                        Endpoint OriginalStart;
                        Endpoint OriginalEnd;
                    };

                    struct ApiMsgGetText : IApiMsg
                    {
                        const wchar_t* Text;
                    };

                    struct ApiMsgMove : IApiMsg
                    {
                        Endpoint OriginalStart;
                        Endpoint OriginalEnd;
                        int RequestedCount;
                        int MovedCount;
                    };

                    struct ApiMsgMoveEndpointByUnit : IApiMsg
                    {
                        Endpoint OriginalStart;
                        Endpoint OriginalEnd;
                        TextPatternRangeEndpoint Endpoint;
                        int RequestedCount;
                        int MovedCount;
                    };

                    struct ApiMsgMoveEndpointByRange : IApiMsg
                    {
                        Endpoint OriginalStart;
                        Endpoint OriginalEnd;
                        TextPatternRangeEndpoint Endpoint;
                        TextPatternRangeEndpoint TargetEndpoint;
                        IdType OtherId;
                    };

                    struct ApiMsgScrollIntoView : IApiMsg
                    {
                        bool AlignToTop;
                    };
                }

            }
        }
    }
}