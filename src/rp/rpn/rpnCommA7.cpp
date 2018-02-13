/** 
 *  Copyright (c) 1999~2017, Altibase Corp. and/or its affiliates. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License, version 3,
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
 

/***********************************************************************
 * $Id: rpnComm.cpp 16602 2006-06-09 01:39:30Z lswhh $
 **********************************************************************/

#include <idl.h>
#include <ide.h>

#include <rpDef.h>
#include <rpuProperty.h>
#include <rpcHBT.h>
#include <rpnComm.h>

IDE_RC rpnComm::sendVersionA7( void               * aHBTResource,
                               cmiProtocolContext * aProtocolContext,
                               rpdVersion         * aReplVersion )
{
    UChar sOpCode;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 8, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Version Set and Send */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Version );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR8( aProtocolContext, &(aReplVersion->mVersion) );

    IDU_FIT_POINT( "rpnComm::sendVersion::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvVersionA7( cmiProtocolContext * aProtocolContext,
                               rpdVersion         * aReplVersion )
{
    UChar sOpCode;
    
    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );
    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, Version ),
                    ERR_CHECK_OPERATION_TYPE );

    /* Version Number Set */
    CMI_RD8( aProtocolContext, &(aReplVersion->mVersion) );

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;    
}

IDE_RC rpnComm::sendMetaDictTableCountA7( void               * aHBTResource,
                                          cmiProtocolContext * aProtocolContext,
                                          SInt               * aDictTableCount )
{
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    /* Validate Parameters */
    IDE_DASSERT( aDictTableCount != NULL );

    IDE_TEST( checkAndFlush( NULL, /* aHBTResource */
                             aProtocolContext,
                             1 + 4,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication Information Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, MetaDictTableCount );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, (UInt *)aDictTableCount );

    IDU_FIT_POINT( "rpnComm::sendMetaDictTableCount::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvMetaDictTableCountA7( cmiProtocolContext * aProtocolContext,
                                          SInt               * aDictTableCount,
                                          ULong                aTimeoutSec )
{
    UChar   sOpCode;

    IDE_TEST( readCmBlock( aProtocolContext, 
                           NULL /* ExitFlag */, 
                           NULL /* TimeoutFlag */, 
                           aTimeoutSec )
              != IDE_SUCCESS );

    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );

    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, MetaDictTableCount ),
                    ERR_CHECK_OPERATION_TYPE );

    CMI_RD4( aProtocolContext, (UInt*)aDictTableCount );

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;    
}


IDE_RC rpnComm::sendMetaReplA7( void               * aHBTResource,
                                cmiProtocolContext * aProtocolContext,
                                rpdReplications    * aRepl )
{
    UInt sRepNameLen;
    UInt sOSInfoLen;
    UInt sCharSetLen;
    UInt sNationalCharSetLen;
    UInt sServerIDLen;
    UInt sRemoteFaultDetectTimeLen;
	
    ULong sDeprecated = -1;

    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    /* Validate Parameters */
    IDE_DASSERT( aRepl != NULL );

    sRepNameLen = idlOS::strlen( aRepl->mRepName );
    sOSInfoLen = idlOS::strlen( aRepl->mOSInfo );
    sCharSetLen = idlOS::strlen( aRepl->mDBCharSet );
    sNationalCharSetLen = idlOS::strlen( aRepl->mNationalCharSet );
    sServerIDLen = idlOS::strlen( aRepl->mServerID );
    sRemoteFaultDetectTimeLen = idlOS::strlen( aRepl->mRemoteFaultDetectTime );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext,
                             1 + 96 + sRepNameLen + sOSInfoLen  + sCharSetLen +
                             sNationalCharSetLen + sServerIDLen + sRemoteFaultDetectTimeLen,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication Information Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, MetaRepl );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &sRepNameLen );
    CMI_WCP( aProtocolContext, (UChar *)aRepl->mRepName, sRepNameLen );
    CMI_WR4( aProtocolContext, (UInt*)&(aRepl->mItemCount) );
    CMI_WR4( aProtocolContext, (UInt*)&(aRepl->mRole) );
    CMI_WR4( aProtocolContext, (UInt*)&(aRepl->mConflictResolution) );
    CMI_WR4( aProtocolContext, &(aRepl->mTransTblSize) );
    CMI_WR4( aProtocolContext, &(aRepl->mFlags) );
    CMI_WR4( aProtocolContext, (UInt*)&(aRepl->mOptions) );
    CMI_WR8( aProtocolContext, &(aRepl->mRPRecoverySN) );
    CMI_WR4( aProtocolContext, &(aRepl->mReplMode) );
    CMI_WR4( aProtocolContext, &(aRepl->mParallelID) );
    CMI_WR4( aProtocolContext, (UInt*)&(aRepl->mIsStarted) );
    /* PROJ-1915 OS, Endian, SMVersion, LFGCount, LogFileSize */
    CMI_WR4( aProtocolContext, &(sOSInfoLen) );
    CMI_WCP( aProtocolContext, (UChar *)aRepl->mOSInfo, sOSInfoLen );
    CMI_WR4( aProtocolContext, &(aRepl->mCompileBit) );
    CMI_WR4( aProtocolContext, &(aRepl->mSmVersionID) );
    CMI_WR4( aProtocolContext, &(aRepl->mLFGCount) );
    CMI_WR8( aProtocolContext, &(aRepl->mLogFileSize) );
    CMI_WR8( aProtocolContext, &( sDeprecated ) );
    /* CharSet Set */
    CMI_WR4( aProtocolContext, &(sCharSetLen) );
    CMI_WCP( aProtocolContext, (UChar *)aRepl->mDBCharSet, sCharSetLen );
    CMI_WR4( aProtocolContext, &(sNationalCharSetLen) );
    CMI_WCP( aProtocolContext, (UChar *)aRepl->mNationalCharSet, sNationalCharSetLen );
    /* Server ID */
    CMI_WR4( aProtocolContext, &(sServerIDLen) );
    CMI_WCP( aProtocolContext, (UChar *)aRepl->mServerID, sServerIDLen );
    CMI_WR4( aProtocolContext, &(sRemoteFaultDetectTimeLen) );
    CMI_WCP( aProtocolContext, (UChar *)aRepl->mRemoteFaultDetectTime, sRemoteFaultDetectTimeLen );

    IDU_FIT_POINT( "rpnComm::sendMetaRepl::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvMetaReplA7( cmiProtocolContext * aProtocolContext,
                                rpdReplications    * aRepl,
                                ULong                aTimeoutSec )
{
    UChar sOpCode;
    UInt sRepNameLen;
    UInt sOSInfoLen;
    UInt sCharSetLen;
    UInt sNationalCharSetLen;
    UInt sServerIDLen;
    UInt sRemoteFaultDetectTimeLen;

    ULong sDeprecated = -1;

    IDE_TEST( readCmBlock( aProtocolContext, NULL /* ExitFlag */, NULL /* TimeoutFlag */, aTimeoutSec )
              != IDE_SUCCESS );

    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );

    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, MetaRepl ),
                    ERR_CHECK_OPERATION_TYPE );

    /* Get Replication Information */
    CMI_RD4( aProtocolContext, &(sRepNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aRepl->mRepName, sRepNameLen );
    aRepl->mRepName[sRepNameLen] = '\0';
    CMI_RD4( aProtocolContext, (UInt*)&(aRepl->mItemCount) );
    CMI_RD4( aProtocolContext, (UInt*)&(aRepl->mRole) );
    CMI_RD4( aProtocolContext, (UInt*)&(aRepl->mConflictResolution) );
    CMI_RD4( aProtocolContext, &(aRepl->mTransTblSize) );
    CMI_RD4( aProtocolContext, &(aRepl->mFlags) );
    CMI_RD4( aProtocolContext, (UInt*)&(aRepl->mOptions) );
    CMI_RD8( aProtocolContext, &(aRepl->mRPRecoverySN) );
    CMI_RD4( aProtocolContext, &(aRepl->mReplMode) );
    CMI_RD4( aProtocolContext, &(aRepl->mParallelID) );
    CMI_RD4( aProtocolContext, (UInt*)&(aRepl->mIsStarted) );
    /* PROJ-1915 OS, Endian, SMVersion, LFGCount, LogFileSize */
    CMI_RD4( aProtocolContext, &(sOSInfoLen) );
    CMI_RCP( aProtocolContext, (UChar *)aRepl->mOSInfo, sOSInfoLen );
    aRepl->mOSInfo[sOSInfoLen] = '\0';
    CMI_RD4( aProtocolContext, &(aRepl->mCompileBit) );
    CMI_RD4( aProtocolContext, &(aRepl->mSmVersionID) );
    CMI_RD4( aProtocolContext, &(aRepl->mLFGCount) );
    CMI_RD8( aProtocolContext, &(aRepl->mLogFileSize) );
    CMI_RD8( aProtocolContext, &( sDeprecated ) );
    /* CharSet Get */
    CMI_RD4( aProtocolContext, &(sCharSetLen) );
    CMI_RCP( aProtocolContext, (UChar *)aRepl->mDBCharSet, sCharSetLen );
    aRepl->mDBCharSet[sCharSetLen] = '\0';
    CMI_RD4( aProtocolContext, &(sNationalCharSetLen) );
    CMI_RCP( aProtocolContext, (UChar *)aRepl->mNationalCharSet, sNationalCharSetLen );
    aRepl->mNationalCharSet[sNationalCharSetLen] = '\0';
    /* Server ID */
    CMI_RD4( aProtocolContext, &(sServerIDLen) );
    CMI_RCP( aProtocolContext, (UChar *)aRepl->mServerID, sServerIDLen );
    aRepl->mServerID[sServerIDLen] = '\0';
    CMI_RD4( aProtocolContext, &(sRemoteFaultDetectTimeLen) );
    CMI_RCP( aProtocolContext, (UChar *)aRepl->mRemoteFaultDetectTime, sRemoteFaultDetectTimeLen );
    aRepl->mRemoteFaultDetectTime[sRemoteFaultDetectTimeLen] = '\0';

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;    
}

IDE_RC rpnComm::sendMetaReplTblA7( void               * aHBTResource,
                                   cmiProtocolContext * aProtocolContext,
                                   rpdMetaItem        * aItem )
{
    UInt sRepNameLen;
    UInt sLocalUserNameLen;
    UInt sLocalTableNameLen;
    UInt sLocalPartNameLen;
    UInt sRemoteUserNameLen;
    UInt sRemoteTableNameLen;
    UInt sRemotePartNameLen;
    UInt sPartCondMinValuesLen;
    UInt sPartCondMaxValuesLen;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    /* Validate Parameters */
    IDE_DASSERT( aItem != NULL );

    sRepNameLen = idlOS::strlen( aItem->mItem.mRepName );
    sLocalUserNameLen = idlOS::strlen( aItem->mItem.mLocalUsername );
    sLocalTableNameLen = idlOS::strlen( aItem->mItem.mLocalTablename );
    sLocalPartNameLen = idlOS::strlen( aItem->mItem.mLocalPartname );
    sRemoteUserNameLen = idlOS::strlen( aItem->mItem.mRemoteUsername );
    sRemoteTableNameLen = idlOS::strlen( aItem->mItem.mRemoteTablename );
    sRemotePartNameLen = idlOS::strlen( aItem->mItem.mRemotePartname );
    sPartCondMinValuesLen  = idlOS::strlen( aItem->mPartCondMinValues );
    sPartCondMaxValuesLen  = idlOS::strlen( aItem->mPartCondMaxValues );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext,
                             1 + 80 + sRepNameLen + sLocalUserNameLen + sLocalTableNameLen +
                             sLocalPartNameLen + sRemoteUserNameLen + sRemoteTableNameLen +
                             sRemotePartNameLen + sPartCondMinValuesLen + sPartCondMaxValuesLen,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication Item Information Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, MetaReplTbl );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sRepNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mItem.mRepName, sRepNameLen );
    CMI_WR4( aProtocolContext, &(sLocalUserNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mItem.mLocalUsername, sLocalUserNameLen );
    CMI_WR4( aProtocolContext, &(sLocalTableNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mItem.mLocalTablename, sLocalTableNameLen );
    CMI_WR4( aProtocolContext, &(sLocalPartNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mItem.mLocalPartname, sLocalPartNameLen );
    CMI_WR4( aProtocolContext, &(sRemoteUserNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mItem.mRemoteUsername, sRemoteUserNameLen );
    CMI_WR4( aProtocolContext, &(sRemoteTableNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mItem.mRemoteTablename, sRemoteTableNameLen );
    CMI_WR4( aProtocolContext, &(sRemotePartNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mItem.mRemotePartname, sRemotePartNameLen );
    CMI_WR4( aProtocolContext, &(sPartCondMinValuesLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mPartCondMinValues, sPartCondMinValuesLen );
    CMI_WR4( aProtocolContext, &(sPartCondMaxValuesLen) );
    CMI_WCP( aProtocolContext, (UChar *)aItem->mPartCondMaxValues, sPartCondMaxValuesLen );
    CMI_WR4( aProtocolContext, &(aItem->mPartitionMethod) );
    CMI_WR4( aProtocolContext, &(aItem->mPartitionOrder) );
    CMI_WR8( aProtocolContext, &(aItem->mItem.mTableOID) );
    CMI_WR4( aProtocolContext, &(aItem->mPKIndex.indexId) );
    CMI_WR4( aProtocolContext, (UInt*)&(aItem->mPKColCount) );
    CMI_WR4( aProtocolContext, (UInt*)&(aItem->mColCount) );
    CMI_WR4( aProtocolContext, (UInt*)&(aItem->mIndexCount) );
    /* PROJ-1915 Invalid Max SN�� ���� �Ѵ�. */
    CMI_WR8( aProtocolContext, &(aItem->mItem.mInvalidMaxSN) );

    /* BUG-34360 Check Constraint */
    CMI_WR4( aProtocolContext, (UInt*)&(aItem->mCheckCount) );

    IDU_FIT_POINT( "rpnComm::sendMetaReplTbl::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext,
                           ID_TRUE )
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvMetaReplTblA7( cmiProtocolContext * aProtocolContext,
                                   rpdMetaItem        * aItem,
                                   ULong                aTimeoutSec )
{
    UChar sOpCode;
    UInt sRepNameLen;
    UInt sLocalUserNameLen;
    UInt sLocalTableNameLen;
    UInt sLocalPartNameLen;
    UInt sRemoteUserNameLen;
    UInt sRemoteTableNameLen;
    UInt sRemotePartNameLen;
    UInt sPartCondMinValuesLen;
    UInt sPartCondMaxValuesLen;

    IDE_TEST( readCmBlock( aProtocolContext, NULL /* ExitFlag */, NULL /* TimeoutFlag */, aTimeoutSec )
              != IDE_SUCCESS );

    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );

    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, MetaReplTbl ),
                    ERR_CHECK_OPERATION_TYPE );

    /* Get Replication Item Information */
    CMI_RD4( aProtocolContext, &(sRepNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mItem.mRepName, sRepNameLen );
    aItem->mItem.mRepName[sRepNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sLocalUserNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mItem.mLocalUsername, sLocalUserNameLen );
    aItem->mItem.mLocalUsername[sLocalUserNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sLocalTableNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mItem.mLocalTablename, sLocalTableNameLen );
    aItem->mItem.mLocalTablename[sLocalTableNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sLocalPartNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mItem.mLocalPartname, sLocalPartNameLen );
    aItem->mItem.mLocalPartname[sLocalPartNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sRemoteUserNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mItem.mRemoteUsername, sRemoteUserNameLen );
    aItem->mItem.mRemoteUsername[sRemoteUserNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sRemoteTableNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mItem.mRemoteTablename, sRemoteTableNameLen );
    aItem->mItem.mRemoteTablename[sRemoteTableNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sRemotePartNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mItem.mRemotePartname, sRemotePartNameLen );
    aItem->mItem.mRemotePartname[sRemotePartNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sPartCondMinValuesLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mPartCondMinValues, sPartCondMinValuesLen );
    aItem->mPartCondMinValues[sPartCondMinValuesLen] = '\0';
    CMI_RD4( aProtocolContext, &(sPartCondMaxValuesLen) );
    CMI_RCP( aProtocolContext, (UChar *)aItem->mPartCondMaxValues, sPartCondMaxValuesLen );
    aItem->mPartCondMaxValues[sPartCondMaxValuesLen] = '\0';
    CMI_RD4( aProtocolContext, &(aItem->mPartitionMethod) );
    CMI_RD4( aProtocolContext, &(aItem->mPartitionOrder) );
    CMI_RD8( aProtocolContext, &(aItem->mItem.mTableOID) );
    CMI_RD4( aProtocolContext, &(aItem->mPKIndex.indexId) );
    CMI_RD4( aProtocolContext, (UInt*)&(aItem->mPKColCount) );
    CMI_RD4( aProtocolContext, (UInt*)&(aItem->mColCount) );
    CMI_RD4( aProtocolContext, (UInt*)&(aItem->mIndexCount) );
    /* PROJ-1915 Invalid Max SN�� ���� �Ѵ�. */
    CMI_RD8( aProtocolContext, &(aItem->mItem.mInvalidMaxSN) );
    
    /* BUG-34260 Check Constraint */
    CMI_RD4( aProtocolContext, (UInt*)&(aItem->mCheckCount) );

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;    
}

IDE_RC rpnComm::sendMetaReplColA7( void               * aHBTResource,
                                   cmiProtocolContext * aProtocolContext,
                                   rpdColumn          * aColumn )
{
    UInt sColumnNameLen;
    UInt sPolicyNameLen;
    UInt sPolicyCodeLen;
    UInt sECCPolicyNameLen;
    UInt sECCPolicyCodeLen;
    UChar sOpCode;
    UInt sDefaultExprLen = 0;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    /* Validate Parameters */
    IDE_DASSERT( aColumn != NULL );

    sColumnNameLen = idlOS::strlen( aColumn->mColumnName );
    sPolicyNameLen = idlOS::strlen( aColumn->mColumn.policy );
    sPolicyCodeLen = idlOS::strlen( (SChar *)aColumn->mPolicyCode );
    sECCPolicyNameLen = idlOS::strlen (aColumn->mECCPolicyName );
    sECCPolicyCodeLen = idlOS::strlen( (SChar *)aColumn->mECCPolicyCode );

    if ( aColumn->mFuncBasedIdxStr != NULL )
    {
        sDefaultExprLen = idlOS::strlen( aColumn->mFuncBasedIdxStr );
    }
    else 
    {
        sDefaultExprLen = 0;
    }

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext,
                             1 + 68 + sColumnNameLen + sPolicyNameLen + sPolicyCodeLen +
                             sECCPolicyNameLen + sECCPolicyCodeLen + sDefaultExprLen,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication Item Information Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, MetaReplCol );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sColumnNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aColumn->mColumnName, sColumnNameLen );
    CMI_WR4( aProtocolContext, &(aColumn->mColumn.column.id) );
    CMI_WR4( aProtocolContext, &(aColumn->mColumn.column.flag) );
    CMI_WR4( aProtocolContext, &(aColumn->mColumn.column.offset) );
    CMI_WR4( aProtocolContext, &(aColumn->mColumn.column.size) );
    CMI_WR4( aProtocolContext, &(aColumn->mColumn.type.dataTypeId) );
    CMI_WR4( aProtocolContext, &(aColumn->mColumn.type.languageId) );
    CMI_WR4( aProtocolContext, &(aColumn->mColumn.flag) );
    CMI_WR4( aProtocolContext, (UInt*)&(aColumn->mColumn.precision) );
    CMI_WR4( aProtocolContext, (UInt*)&(aColumn->mColumn.scale) );
    CMI_WR4( aProtocolContext, (UInt*)&(aColumn->mColumn.encPrecision) );
    CMI_WR4( aProtocolContext, &(sPolicyNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aColumn->mColumn.policy, sPolicyNameLen );
    CMI_WR4( aProtocolContext, &(sPolicyCodeLen) );
    CMI_WCP( aProtocolContext, (UChar *)aColumn->mPolicyCode, sPolicyCodeLen );
    CMI_WR4( aProtocolContext, &(sECCPolicyNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aColumn->mECCPolicyName, sECCPolicyNameLen );
    CMI_WR4( aProtocolContext, &(sECCPolicyCodeLen) );
    CMI_WCP( aProtocolContext, (UChar *)aColumn->mECCPolicyCode, sECCPolicyCodeLen );

    /* BUG-35210 Function-based index */
    CMI_WR4( aProtocolContext, &(aColumn->mQPFlag) );
    CMI_WR4( aProtocolContext, &(sDefaultExprLen) );
    if ( sDefaultExprLen != 0 )
    {
        CMI_WCP( aProtocolContext, (UChar *)aColumn->mFuncBasedIdxStr, sDefaultExprLen );
    }
    else
    {
        /* do nothing */
    }

    IDU_FIT_POINT( "rpnComm::sendMetaReplCol::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvMetaReplColA7( cmiProtocolContext * aProtocolContext,
                                   rpdColumn          * aColumn,
                                   ULong                aTimeoutSec )
{
    UChar sOpCode;
    UInt sColumnNameLen;
    UInt sPolicyNameLen;
    UInt sPolicyCodeLen;
    UInt sECCPolicyNameLen;
    UInt sECCPolicyCodeLen;
    UInt sDefaultExprLen = 0;

    IDE_TEST( readCmBlock( aProtocolContext, NULL /* ExitFlag */, NULL /* TimeoutFlag */, aTimeoutSec )
              != IDE_SUCCESS );

    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );

    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, MetaReplCol ),
                    ERR_CHECK_OPERATION_TYPE );
    /* Get Replication Item Column Information */
    CMI_RD4( aProtocolContext, &(sColumnNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aColumn->mColumnName, sColumnNameLen );
    aColumn->mColumnName[sColumnNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(aColumn->mColumn.column.id) );
    CMI_RD4( aProtocolContext, &(aColumn->mColumn.column.flag) );
    CMI_RD4( aProtocolContext, &(aColumn->mColumn.column.offset) );
    CMI_RD4( aProtocolContext, &(aColumn->mColumn.column.size) );
    CMI_RD4( aProtocolContext, &(aColumn->mColumn.type.dataTypeId) );
    CMI_RD4( aProtocolContext, &(aColumn->mColumn.type.languageId) );
    CMI_RD4( aProtocolContext, &(aColumn->mColumn.flag) );
    CMI_RD4( aProtocolContext, (UInt*)&(aColumn->mColumn.precision) );
    CMI_RD4( aProtocolContext, (UInt*)&(aColumn->mColumn.scale) );
    CMI_RD4( aProtocolContext, (UInt*)&(aColumn->mColumn.encPrecision) );
    CMI_RD4( aProtocolContext, &(sPolicyNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aColumn->mColumn.policy, sPolicyNameLen );
    aColumn->mColumn.policy[sPolicyNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sPolicyCodeLen) );
    CMI_RCP( aProtocolContext, (UChar *)aColumn->mPolicyCode, sPolicyCodeLen );
    aColumn->mPolicyCode[sPolicyCodeLen] = '\0';
    CMI_RD4( aProtocolContext, &(sECCPolicyNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aColumn->mECCPolicyName, sECCPolicyNameLen );
    aColumn->mECCPolicyName[sECCPolicyNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(sECCPolicyCodeLen) );
    CMI_RCP( aProtocolContext, (UChar *)aColumn->mECCPolicyCode, sECCPolicyCodeLen );
    aColumn->mECCPolicyCode[sECCPolicyCodeLen] = '\0';

    /* BUG-35210 Function-based Index */
    CMI_RD4( aProtocolContext, &(aColumn->mQPFlag) );
    CMI_RD4( aProtocolContext, &(sDefaultExprLen) );
    if ( sDefaultExprLen != 0 )
    {
        IDU_FIT_POINT_RAISE( "rpnComm::recvMetaReplColA7::calloc::FuncBasedIndexExpr",
                              ERR_MEMORY_ALLOC );
        IDE_TEST_RAISE( iduMemMgr::calloc( IDU_MEM_RP_RPN,
                                           sDefaultExprLen + 1,
                                           ID_SIZEOF(SChar),
                                           (void **)&(aColumn->mFuncBasedIdxStr),
                                           IDU_MEM_IMMEDIATE )
                        != IDE_SUCCESS, ERR_MEMORY_ALLOC );
        CMI_RCP( aProtocolContext, (UChar *)aColumn->mFuncBasedIdxStr, sDefaultExprLen );
        aColumn->mFuncBasedIdxStr[sDefaultExprLen] = '\0';
    }
    else
    {
        /* do nothing */
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }

    IDE_EXCEPTION( ERR_MEMORY_ALLOC );
    {
        IDE_ERRLOG( IDE_RP_0 );
        IDE_SET( ideSetErrorCode( rpERR_ABORT_MEMORY_ALLOC,
                                  "rpnComm::recvMetaReplColA7",
                                  "aColumn->mFuncBasedIdxStr" ) );
        IDE_ERRLOG( IDE_RP_0 );
    }

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendMetaReplIdxA7( void               * aHBTResource,
                                   cmiProtocolContext * aProtocolContext,
                                   qcmIndex           * aIndex )
{
    UInt sNameLen;
    UInt sTempValue;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    /* Validate Parameters */
    IDE_DASSERT( aIndex != NULL );

    sNameLen = idlOS::strlen( aIndex->name );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 24 + sNameLen,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication Item Index Information Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, MetaReplIdx );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aIndex->name, sNameLen );
    CMI_WR4( aProtocolContext, &(aIndex->indexId) );
    CMI_WR4( aProtocolContext, &(aIndex->indexTypeId) );
    CMI_WR4( aProtocolContext, &(aIndex->keyColCount) );

    sTempValue = aIndex->isUnique;
    CMI_WR4( aProtocolContext, &(sTempValue) );

    sTempValue = aIndex->isRange;
    CMI_WR4( aProtocolContext, &(sTempValue) );

    IDU_FIT_POINT( "rpnComm::sendMetaReplIdx::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvMetaReplIdxA7( cmiProtocolContext * aProtocolContext,
                                   qcmIndex           * aIndex,
                                   ULong                aTimeoutSec )
{
    UChar sOpCode;
    UInt  sNameLen;
    UInt  sIsUnique;
    UInt  sIsRange;

    IDE_TEST( readCmBlock( aProtocolContext,
                           NULL /* ExitFlag */,
                           NULL /* TimeoutFlag */,
                           aTimeoutSec )
              != IDE_SUCCESS );

    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );

    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, MetaReplIdx ),
                    ERR_CHECK_OPERATION_TYPE );

    CMI_RD4( aProtocolContext, &(sNameLen) );
    CMI_RCP( aProtocolContext, (UChar *)aIndex->name, sNameLen );
    aIndex->name[sNameLen] = '\0';
    CMI_RD4( aProtocolContext, &(aIndex->indexId) );
    CMI_RD4( aProtocolContext, &(aIndex->indexTypeId) );
    CMI_RD4( aProtocolContext, &(aIndex->keyColCount) );
    CMI_RD4( aProtocolContext, &(sIsUnique) );
    CMI_RD4( aProtocolContext, &(sIsRange) );

    aIndex->isUnique = ( sIsUnique == 0 ) ? ID_FALSE : ID_TRUE;
    aIndex->isRange = ( sIsRange == 0 ) ? ID_FALSE : ID_TRUE;

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendMetaReplIdxColA7( void                * aHBTResource,
                                      cmiProtocolContext  * aProtocolContext,
                                      UInt                  aColumnID,
                                      UInt                  aKeyColumnFlag )
{
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 8, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication Item Index Column Description Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, MetaReplIdxCol );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(aColumnID) );
    CMI_WR4( aProtocolContext, &(aKeyColumnFlag) );

    IDU_FIT_POINT( "rpnComm::sendMetaReplIdxCol::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvMetaReplIdxColA7( cmiProtocolContext * aProtocolContext,
                                      UInt               * aColumnID,
                                      UInt               * aKeyColumnFlag,
                                      ULong                aTimeoutSec )
{
    UChar sOpCode;

    IDE_TEST( readCmBlock( aProtocolContext, NULL /* ExitFlag */, NULL /* TimeoutFlag */, aTimeoutSec )
              != IDE_SUCCESS );

    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );

    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, MetaReplIdxCol ),
                    ERR_CHECK_OPERATION_TYPE );

    /* Get Replication Item Index Information */
    CMI_RD4( aProtocolContext, aColumnID );
    CMI_RD4( aProtocolContext, aKeyColumnFlag );

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendMetaReplCheckA7( void                 * aHBTResource,
                                     cmiProtocolContext   * aProtocolContext,
                                     qcmCheck             * aCheck ) 
{
    UInt    sCheckConditionLength = 0;
    UInt    sColumnCount = 0;
    UInt    sCheckNameLength = 0;
    UInt    sColumnIndex = 0;
    UChar   sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    /* Validate Parameters */
    sCheckNameLength = idlOS::strlen( aCheck->name );
    sColumnCount = aCheck->constraintColumnCount;
    sCheckConditionLength = idlOS::strlen( aCheck->checkCondition );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 16 + sCheckNameLength + 
                             sColumnCount * 4 + sCheckConditionLength,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication Item Check Information Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, MetaReplCheck );

    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &sCheckNameLength );
    CMI_WCP( aProtocolContext, (UChar *)aCheck->name, sCheckNameLength );

    CMI_WR4( aProtocolContext, &(aCheck->constraintID) );
    CMI_WR4( aProtocolContext, &(aCheck->constraintColumnCount) );

    for ( sColumnIndex = 0; sColumnIndex < aCheck->constraintColumnCount; sColumnIndex++ )
    {
        CMI_WR4( aProtocolContext, &(aCheck->constraintColumn[sColumnIndex]) );
    }

    CMI_WR4( aProtocolContext, &sCheckConditionLength );
    CMI_WCP( aProtocolContext, (UChar *)aCheck->checkCondition, sCheckConditionLength );

    IDU_FIT_POINT( "rpnComm::sendMetaReplCheck::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvMetaReplCheckA7( cmiProtocolContext   * aProtocolContext,
                                     qcmCheck             * aCheck,
                                     const ULong            aTimeoutSec )
{
    UChar       sOpCode;
    UInt        sCheckNameLength = 0;
    UInt        sCheckConditionLength = 0; 
    UInt        sColumnIndex = 0;

    IDE_TEST( readCmBlock( aProtocolContext, 
                           NULL /* ExitFlag */, 
                           NULL /* TimeoutFlag */, 
                           aTimeoutSec )
              != IDE_SUCCESS );

    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );

    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, MetaReplCheck ),
                    ERR_CHECK_OPERATION_TYPE );

    /* Get Replication Item check Information */
    CMI_RD4( aProtocolContext, &sCheckNameLength );
    CMI_RCP( aProtocolContext, (UChar *)aCheck->name, sCheckNameLength );
    aCheck->name[sCheckNameLength] = '\0';

    CMI_RD4( aProtocolContext, &(aCheck->constraintID) );
    CMI_RD4( aProtocolContext, &(aCheck->constraintColumnCount) );

    for ( sColumnIndex = 0; sColumnIndex < aCheck->constraintColumnCount; sColumnIndex++ )
    {
        CMI_RD4( aProtocolContext, &(aCheck->constraintColumn[sColumnIndex]) );
    }

    CMI_RD4( aProtocolContext, &sCheckConditionLength );

    IDU_FIT_POINT_RAISE( "rpnComm::recvMetaReplCheckA7::calloc::CheckCondition", 
                          ERR_MEMORY_ALLOC_CHECK );
    IDE_TEST_RAISE( iduMemMgr::calloc( IDU_MEM_RP_RPN,
                                       sCheckConditionLength + 1,
                                       ID_SIZEOF(SChar),
                                       (void **)&(aCheck->checkCondition),
                                       IDU_MEM_IMMEDIATE )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC_CHECK );

    CMI_RCP( aProtocolContext, (UChar *)aCheck->checkCondition, sCheckConditionLength );
    aCheck->checkCondition[sCheckConditionLength] = '\0';

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }

    IDE_EXCEPTION( ERR_MEMORY_ALLOC_CHECK );
    {
        IDE_ERRLOG( IDE_RP_0 );
        IDE_SET( ideSetErrorCode( rpERR_ABORT_MEMORY_ALLOC,
                                  "rpnComm::recvMetaReplCheckA7",
                                  "aChecks" ) );
        IDE_ERRLOG( IDE_RP_0 );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendHandshakeAckA7( cmiProtocolContext  * aProtocolContext,
                                    UInt                  aResult,
                                    SInt                  aFailbackStatus,
                                    ULong                 aXSN,
                                    SChar               * aMsg )
{
    UInt sMsgLen = 0;
    UChar sOpCode = 0;
    ULong sVersion = RP_CURRENT_VERSION;

    if ( aResult == RP_MSG_OK )
    {
        // copy protocol version.
        sMsgLen = ID_SIZEOF(ULong);
    }
    else
    {
        sMsgLen = idlOS::strlen( aMsg ) + 1;
    }

    IDE_TEST( checkAndFlush( NULL,  /* aHBTResource */
                             aProtocolContext, 
                             1 + 12 + sMsgLen,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication Item Information Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, HandshakeAck );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &aResult );
    CMI_WR4( aProtocolContext, (UInt*)&aFailbackStatus );
    CMI_WR8( aProtocolContext, (ULong*)&aXSN );
    CMI_WR4( aProtocolContext, &sMsgLen );

    if ( aResult == RP_MSG_OK )
    {
        CMI_WR8( aProtocolContext, (ULong*)&sVersion );
    }
    else
    {
        CMI_WCP( aProtocolContext, (UChar *)aMsg, sMsgLen );
    }

    IDU_FIT_POINT( "rpnComm::sendHandshakeAckA7::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( NULL, /* aHBTResource */
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvHandshakeAckA7( cmiProtocolContext * aProtocolContext,
                                    idBool             * aExitFlag,
                                    UInt               * aResult,
                                    SInt               * aFailbackStatus,
                                    ULong              * aXSN,
                                    SChar              * aMsg,
                                    UInt               * aMsgLen,
                                    ULong                aTimeOut )
{
    UChar sOpCode;

    IDE_TEST( readCmBlock( aProtocolContext, aExitFlag, NULL /* TimeoutFlag */, aTimeOut )
              != IDE_SUCCESS );

    /*
     * recvHandshakeAck()�� ���� ó������ ��Ŷ�� �޴� ��찡 �ִ�.
     * �׷��� readCmBlock() ���Ŀ� �˻縦 �Ѵ�.
     */ 
    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );
    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, HandshakeAck ),
                    ERR_CHECK_OPERATION_TYPE );

    /* Get argument */
    CMI_RD4( aProtocolContext, aResult );
    CMI_RD4( aProtocolContext, (UInt*)aFailbackStatus );
    CMI_RD8( aProtocolContext, (ULong*)aXSN );
    CMI_RD4( aProtocolContext, aMsgLen );
    CMI_RCP( aProtocolContext, (UChar *)aMsg, *aMsgLen );
    aMsg[*aMsgLen] = '\0';

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvXLogA7( iduMemAllocator    * aAllocator,
                            cmiProtocolContext * aProtocolContext,
                            idBool             * aExitFlag,
                            rpdMeta            * aMeta,
                            rpdXLog            * aXLog,
                            ULong                aTimeOutSec )
{
    UChar sOpCode;

    if ( CMI_IS_READ_ALL( aProtocolContext ) == ID_TRUE )
    {
        IDE_TEST( readCmBlock( aProtocolContext, aExitFlag, NULL /* TimeoutFlag */, aTimeOutSec )
                  != IDE_SUCCESS );
    }

    CMI_RD1( aProtocolContext, sOpCode );

    // BUG-17215
    IDU_FIT_POINT_RAISE( "1.BUG-17215@rpnComm::recvXLogA7",
                          ERR_PROTOCOL_OPID );

    switch ( sOpCode )
    {
        case CMI_PROTOCOL_OPERATION( RP, TrBegin ):
        {
            IDE_TEST( recvTrBeginA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, TrCommit ):
        {
            IDE_TEST( recvTrCommitA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, TrAbort ):
        {
            IDE_TEST( recvTrAbortA7( aProtocolContext, aXLog )
                     != IDE_SUCCESS);
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, SPSet ):
        {
            IDE_TEST( recvSPSetA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, SPAbort ):
        {
            IDE_TEST( recvSPAbortA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, Insert ):
        {
            IDE_TEST( recvInsertA7( aAllocator,
                                    aExitFlag,
                                    aProtocolContext,
                                    aMeta,     // BUG-20506
                                    aXLog,
                                    aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, Update ):
        {
            IDE_TEST( recvUpdateA7( aAllocator,
                                    aExitFlag,
                                    aProtocolContext,
                                    aMeta,     // BUG-20506
                                    aXLog,
                                    aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, Delete ):
        {
            IDE_TEST( recvDeleteA7( aAllocator,
                                    aExitFlag,
                                    aProtocolContext,
                                    aMeta,     // BUG-20506
                                    aXLog,
                                    aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, Stop ):
        {
            IDE_TEST( recvStopA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, KeepAlive ):
        {
            IDE_TEST( recvKeepAliveA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, Flush ):
        {
            IDE_TEST( recvFlushA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, LobCursorOpen ):
        {
            IDE_TEST( recvLobCursorOpenA7( aAllocator,
                                           aExitFlag,
                                           aProtocolContext,
                                           aMeta,  // BUG-20506
                                           aXLog,
                                           aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, LobCursorClose ):
        {
            IDE_TEST( recvLobCursorCloseA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS);
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, LobPrepare4Write ):
        {
            IDE_TEST( recvLobPrepare4WriteA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, LobPartialWrite ):
        {
            IDE_TEST( recvLobPartialWriteA7( aAllocator,
                                             aExitFlag,
                                             aProtocolContext,
                                             aXLog,
                                             aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, LobFinish2Write ):
        {
            IDE_TEST( recvLobFinish2WriteA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, LobTrim ):
        {

            IDE_TEST( recvLobTrim( aAllocator,
                                   aExitFlag,
                                   aProtocolContext,
                                   aXLog,
                                   aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, Handshake ): // BUG-23195
        {
            IDE_TEST( recvHandshakeA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, SyncPKBegin ):
        {
            IDE_TEST( recvSyncPKBeginA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, SyncPK ):
        {
            IDE_TEST( recvSyncPKA7( aExitFlag,
                                    aProtocolContext,
                                    aXLog,
                                    aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, SyncPKEnd ):
        {
            IDE_TEST( recvSyncPKEndA7( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION(RP, FailbackEnd):
        {
            IDE_TEST( recvFailbackEndA7( aProtocolContext, aXLog )
                     != IDE_SUCCESS);
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, SyncStart ):
        {
            IDE_TEST( recvSyncStart( aAllocator,
                                     aExitFlag,
                                     aProtocolContext,
                                     aXLog,
                                     aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, SyncRebuildIndex ):
        {
            IDE_TEST( recvRebuildIndex( aAllocator,
                                        aExitFlag,
                                        aProtocolContext,
                                        aXLog,
                                        aTimeOutSec )
                      != IDE_SUCCESS );
            break;
        }
        case CMI_PROTOCOL_OPERATION( RP, AckOnDML ):
        {
            IDE_TEST( recvAckOnDML( aProtocolContext, aXLog )
                      != IDE_SUCCESS );
            break;
        }
        default:
            // BUG-17215
            IDE_RAISE( ERR_PROTOCOL_OPID );
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_PROTOCOL_OPID );
    {
        IDE_SET( ideSetErrorCode(rpERR_ABORT_RECEIVER_UNEXPECTED_PROTOCOL,
                                 sOpCode ) );
    }

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendTrBeginA7( void                * aHBTResource,
                               cmiProtocolContext  * aProtocolContext,
                               smTID                 aTID,
                               smSN                  aSN,
                               smSN                  aSyncSN )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    sType= RP_X_BEGIN;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext,
                             1 + 24,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, TrBegin );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvTrBeginA7( cmiProtocolContext  * aProtocolContext,
                               rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendTrCommitA7( void               * aHBTResource,
                                cmiProtocolContext * aProtocolContext,
                                smTID                aTID,
                                smSN                 aSN,
                                smSN                 aSyncSN,
                                idBool               aForceFlush )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType= RP_X_COMMIT;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 24, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, TrCommit );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );

    if( aForceFlush == ID_TRUE )
    {
        IDU_FIT_POINT( "rpnComm::sendTrCommit::cmiSend::ERR_SEND" );
        IDE_TEST( sendCmBlock( aHBTResource,
                               aProtocolContext, 
                               ID_TRUE ) 
                  != IDE_SUCCESS );
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvTrCommitA7( cmiProtocolContext  * aProtocolContext,
                                rpdXLog             * aXLog )

{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendTrAbortA7( void               * aHBTResource,
                               cmiProtocolContext * aProtocolContext,
                               smTID                aTID,
                               smSN                 aSN,
                               smSN                 aSyncSN )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    sType= RP_X_ABORT;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext,
                             1 + 24,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, TrAbort );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvTrAbortA7( cmiProtocolContext  * aProtocolContext,
                               rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendSPSetA7( void               * aHBTResource,
                             cmiProtocolContext * aProtocolContext,
                             smTID                aTID,
                             smSN                 aSN,
                             smSN                 aSyncSN,
                             UInt                 aSPNameLen,
                             SChar              * aSPName )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType= RP_X_SP_SET;
    
    /* Validate Parameters */
    IDE_DASSERT( aSPNameLen != 0 );
    IDE_DASSERT( aSPName != NULL );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 28 + 
                             aSPNameLen,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, SPSet );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    CMI_WR4( aProtocolContext, &(aSPNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aSPName, aSPNameLen );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvSPSetA7( cmiProtocolContext  * aProtocolContext,
                             rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    CMI_RD4( aProtocolContext, &(aXLog->mSPNameLen) );

    IDE_DASSERT( aXLog->mSPNameLen > 0 );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mSPNameLen + 1,
                                          (void **)&( aXLog->mSPName ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    CMI_RCP( aProtocolContext, (UChar *)aXLog->mSPName, aXLog->mSPNameLen );
    aXLog->mSPName[aXLog->mSPNameLen] = '\0';

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_MEMORY_ALLOC );
    {
        IDE_ERRLOG( IDE_RP_0 );
        IDE_SET( ideSetErrorCode( rpERR_ABORT_MEMORY_ALLOC,
                                  "rpnCommA7::recvSPSetA7",
                                  "mSPName" ) );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendSPAbortA7( void               * aHBTResource,
                               cmiProtocolContext * aProtocolContext,
                               smTID                aTID,
                               smSN                 aSN,
                               smSN                 aSyncSN,
                               UInt                 aSPNameLen,
                               SChar              * aSPName )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType= RP_X_SP_ABORT;

    /* Validate Parameters */
    IDE_DASSERT( aSPNameLen != 0 );
    IDE_DASSERT( aSPName != NULL );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 28 + 
                             aSPNameLen,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, SPAbort );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    CMI_WR4( aProtocolContext, &(aSPNameLen) );
    CMI_WCP( aProtocolContext, (UChar *)aSPName, aSPNameLen );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvSPAbortA7( cmiProtocolContext  * aProtocolContext,
                               rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    CMI_RD4( aProtocolContext, &(aXLog->mSPNameLen) );

    IDE_DASSERT( aXLog->mSPNameLen > 0 );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mSPNameLen + 1,
                                          (void **)&( aXLog->mSPName ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    CMI_RCP( aProtocolContext, (UChar *)aXLog->mSPName, aXLog->mSPNameLen );
    aXLog->mSPName[aXLog->mSPNameLen] = '\0';

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_MEMORY_ALLOC );
    {
        IDE_ERRLOG( IDE_RP_0 );
        IDE_SET( ideSetErrorCode( rpERR_ABORT_MEMORY_ALLOC,
                                  "rpnCommA7::recvSPAbortA7",
                                  "mSPName" ) );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendInsertA7( void               * aHBTResource,
                              cmiProtocolContext * aProtocolContext,
                              smTID                aTID,
                              smSN                 aSN,
                              smSN                 aSyncSN,
                              UInt                 aImplSPDepth,
                              ULong                aTableOID,
                              UInt                 aColCnt,
                              smiValue           * aACols,
                              rpValueLen         * aALen )
{
    UInt i;
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType= RP_X_INSERT;

    /* Validate Parameters */
    IDE_DASSERT(aImplSPDepth <= SMI_STATEMENT_DEPTH_MAX);
    IDE_DASSERT(aColCnt <= QCI_MAX_COLUMN_COUNT);
    IDE_DASSERT(aACols != NULL);

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 40, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Insert );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information     */
    CMI_WR4( aProtocolContext, &(aImplSPDepth) );
    CMI_WR8( aProtocolContext, &(aTableOID) );
    CMI_WR4( aProtocolContext, &(aColCnt) );

    /* Send Value repeatedly */
    for(i = 0; i < aColCnt; i ++)
    {
        IDE_TEST( sendValueForA7( aHBTResource,
                                  aProtocolContext,
                                  &aACols[i],
                                  aALen[i])
                  != IDE_SUCCESS );
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvInsertA7( iduMemAllocator    * aAllocator,
                              idBool             * aExitFlag,
                              cmiProtocolContext * aProtocolContext,
                              rpdMeta            * aMeta,  // BUG-20506
                              rpdXLog            * aXLog,
                              ULong                aTimeOutSec )
{
    UInt             i;
    rpdMetaItem     *sItem   = NULL;
    rpdColumn       *sColumn = NULL;
    const mtdModule *sMtd    = NULL;
    smiValue         sDummy;
    UInt             sCID;
    UInt           * sType = (UInt*)&(aXLog->mType);

    IDE_ASSERT(aMeta != NULL);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    CMI_RD4( aProtocolContext, &(aXLog->mImplSPDepth) );
    CMI_RD8( aProtocolContext, &(aXLog->mTableOID) );
    CMI_RD4( aProtocolContext, &(aXLog->mColCnt) );

    /* PROJ-1442 Replication Online �� DDL ���
     * XLog ���� Column ������ ��迭�Ѵ�.
     *
     * Column : mColCnt, mCIDs[], mACols[]
     */
    (void)aMeta->searchRemoteTable(&sItem, aXLog->mTableOID);

    IDU_FIT_POINT_RAISE( "rpnComm::recvInsertA7::Erratic::rpERR_ABORT_RECEIVER_NOT_FOUND_TABLE",
                         ERR_NOT_FOUND_TABLE );
    IDE_TEST_RAISE(sItem == NULL, ERR_NOT_FOUND_TABLE);

    IDE_TEST_RAISE( aXLog->mMemory.alloc( sItem->mColCount * ID_SIZEOF( UInt ),
                                          (void **)&( aXLog->mCIDs ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( sItem->mColCount * ID_SIZEOF( smiValue ),
                                          (void **)&( aXLog->mBCols ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( sItem->mColCount * ID_SIZEOF( smiValue ),
                                          (void **)&( aXLog->mACols ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    /* Recv Value repeatedly */
    // Column ID ������ Column ���� ��迭�Ѵ�.
    for(i = 0; i < aXLog->mColCnt; i++)
    {
        sCID = sItem->mMapColID[i];

        if( sCID != RP_INVALID_COLUMN_ID )
        {
            aXLog->mCIDs[sCID] = sCID;

            IDE_TEST(recvValueA7( aAllocator,
                                  aExitFlag,
                                  aProtocolContext,
                                  &aXLog->mMemory,
                                  &aXLog->mACols[sCID],
                                  aTimeOutSec)
                     != IDE_SUCCESS);
        }
        else
        {
            sDummy.value = NULL;
            IDE_TEST(recvValueA7(aAllocator,
                                 aExitFlag,
                                 aProtocolContext,
                                 NULL, /* aMemory */
                                 &sDummy,
                                 aTimeOutSec)
                     != IDE_SUCCESS);

            if(sDummy.value != NULL)
            {
                (void)iduMemMgr::free((void *)sDummy.value, aAllocator);
            }
        }
    }

    // Standby Server���� Replication ����� �ƴ� Column�� NULL�� �Ҵ��Ѵ�.
    if(sItem->mHasOnlyReplCol != ID_TRUE)
    {
        for(i = 0; i < (UInt)sItem->mColCount; i++)
        {
            // ��迭�� Column�� XLog�� �����Ѵ�.
            aXLog->mCIDs[i] = i;

            // Replication ����� �ƴ� Column��,
            // DEFAULT �� ��� mtdModule�� staticNull�� ����Ѵ�.
            if(sItem->mIsReplCol[i] != ID_TRUE)
            {
                sColumn = sItem->getRpdColumn(i);
                IDE_TEST_RAISE(sColumn == NULL, ERR_NOT_FOUND_COLUMN);

                IDU_FIT_POINT_RAISE( "rpnComm::recvInsertA7::Erratic::rpERR_ABORT_GET_MODULE",
                                     ERR_GET_MODULE );
                IDE_TEST_RAISE(mtd::moduleById(&sMtd,
                                               sColumn->mColumn.type.dataTypeId)
                               != IDE_SUCCESS, ERR_GET_MODULE);

                IDE_TEST(allocNullValue(aAllocator,
                                        &aXLog->mMemory,
                                        &aXLog->mACols[i],
                                        (const mtcColumn *)&sColumn->mColumn,
                                        sMtd)
                         != IDE_SUCCESS);
            }
        }
    }

    aXLog->mColCnt = sItem->mColCount;

    return IDE_SUCCESS;

    IDE_EXCEPTION(ERR_NOT_FOUND_TABLE);
    {
        IDE_SET(ideSetErrorCode(rpERR_ABORT_RECEIVER_NOT_FOUND_TABLE,
                                aXLog->mSN,
                                aXLog->mTableOID));
    }
    IDE_EXCEPTION(ERR_NOT_FOUND_COLUMN);
    {
        IDE_SET(ideSetErrorCode(rpERR_ABORT_RECEIVER_NOT_FOUND_COLUMN,
                                aXLog->mSN,
                                aXLog->mTableOID,
                                i));
    }
    IDE_EXCEPTION(ERR_GET_MODULE);
    {
        IDE_SET(ideSetErrorCode(rpERR_ABORT_GET_MODULE));
    }
    IDE_EXCEPTION( ERR_MEMORY_ALLOC );
    {
		IDE_ERRLOG( IDE_RP_0 );
        IDE_SET(ideSetErrorCode(rpERR_ABORT_MEMORY_ALLOC,
                                "rpnComm::recvInsertA7",
                                "Column"));        
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendUpdateA7( void            * aHBTResource,
                              cmiProtocolContext *aProtocolContext,
                              smTID             aTID,
                              smSN              aSN,
                              smSN              aSyncSN,
                              UInt              aImplSPDepth,
                              ULong             aTableOID,
                              UInt              aPKColCnt,
                              UInt              aUpdateColCnt,
                              smiValue        * aPKCols,
                              UInt            * aCIDs,
                              smiValue        * aBCols,
                              smiChainedValue * aBChainedCols, // PROJ-1705
                              UInt            * aBChainedColsTotalLen, /* BUG-33722 */
                              smiValue        * aACols,
                              rpValueLen      * aPKLen,
                              rpValueLen      * aALen,
                              rpValueLen      * aBMtdValueLen )
{
    UInt i;
    smiChainedValue * sChainedValue = NULL;
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType= RP_X_UPDATE;

    /* Validate Parameters */
    IDE_DASSERT( aImplSPDepth <= SMI_STATEMENT_DEPTH_MAX );
    IDE_DASSERT( aPKColCnt <= QCI_MAX_KEY_COLUMN_COUNT );
    IDE_DASSERT( aUpdateColCnt <= QCI_MAX_COLUMN_COUNT );
    IDE_DASSERT( aPKCols != NULL );
    IDE_DASSERT( aCIDs != NULL );
    IDE_DASSERT( aBCols != NULL );
    IDE_DASSERT( aACols != NULL );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 44, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Update );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information     */
    CMI_WR4( aProtocolContext, &(aImplSPDepth) );
    CMI_WR8( aProtocolContext, &(aTableOID) );
    CMI_WR4( aProtocolContext, &(aPKColCnt) );
    CMI_WR4( aProtocolContext, &(aUpdateColCnt) );

    /* Send Primary Key Value */
    for ( i = 0; i < aPKColCnt; i ++ )
    {
        IDE_TEST( sendPKValueForA7( aHBTResource,
                                    aProtocolContext,
                                    &aPKCols[i],
                                    aPKLen[i] )
                  != IDE_SUCCESS );
    }

    /* Send Update Column Value : Before/After */
    for ( i = 0; i < aUpdateColCnt; i ++ )
    {
        IDE_TEST( sendCIDForA7( aHBTResource,
                                aProtocolContext,
                                aCIDs[i] )
                  != IDE_SUCCESS );

        // PROJ-1705
        sChainedValue = &aBChainedCols[aCIDs[i]];

        // �޸� ���̺��� �߻��� update�� �м� ����� BCols�� ����ִ�.
        if ( (sChainedValue->mAllocMethod == SMI_NON_ALLOCED) && // mAllocMethod�� �ʱⰪ�� SMI_NON_ALLOCED
             (aBMtdValueLen[aCIDs[i]].lengthSize == 0) )
        {
            IDE_TEST( sendValueForA7( aHBTResource,
                                      aProtocolContext,
                                      &aBCols[aCIDs[i]],
                                      aBMtdValueLen[aCIDs[i]] )
                      != IDE_SUCCESS );
        }
        // ��ũ ���̺��� �߻��� update�� �м� ����� BChainedCols�� ����ִ�.
        else
        {
            IDE_TEST( sendChainedValueForA7( aHBTResource,
                                             aProtocolContext,
                                             sChainedValue,
                                             aBMtdValueLen[aCIDs[i]],
                                             aBChainedColsTotalLen[aCIDs[i]] )
                      != IDE_SUCCESS );
        }

        IDE_TEST( sendValueForA7( aHBTResource,
                                  aProtocolContext,
                                  &aACols[aCIDs[i]],
                                  aALen[aCIDs[i]] )
                  != IDE_SUCCESS );
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvUpdateA7( iduMemAllocator    * aAllocator,
                              idBool             * aExitFlag,
                              cmiProtocolContext * aProtocolContext,
                              rpdMeta            * aMeta,  // BUG-20506
                              rpdXLog            * aXLog,
                              ULong                aTimeOutSec )
{
    UInt i;
    rpdMetaItem *sItem = NULL;
    UInt * sType = (UInt*)&(aXLog->mType);
    smiValue     sDummy;

    IDE_ASSERT( aMeta != NULL );

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Argument XLog Body */
    CMI_RD4( aProtocolContext, &(aXLog->mImplSPDepth) );
    CMI_RD8( aProtocolContext, &(aXLog->mTableOID) );
    CMI_RD4( aProtocolContext, &(aXLog->mPKColCnt) );
    CMI_RD4( aProtocolContext, &(aXLog->mColCnt) );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mPKColCnt * ID_SIZEOF( smiValue ),
                                          (void **)&( aXLog->mPKCols ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mColCnt * ID_SIZEOF( UInt ),
                                          (void **)&( aXLog->mCIDs ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mColCnt * ID_SIZEOF( smiValue),
                                          (void **)&( aXLog->mBCols ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mColCnt * ID_SIZEOF( smiValue ),
                                          (void **)&( aXLog->mACols ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    /* Recv PK Value repeatedly */
    for ( i = 0; i < aXLog->mPKColCnt; i ++ )
    {
        IDE_TEST(recvValueA7(aAllocator,
                             aExitFlag,
                             aProtocolContext,
                             &aXLog->mMemory,
                             &aXLog->mPKCols[i],
                             aTimeOutSec)
                 != IDE_SUCCESS );
    }

    /* PROJ-1442 Replication Online �� DDL ���
     * XLog ���� Column ������ ��迭�Ѵ�.
     *   - mPKColCnt�� ������ �����Ƿ�, �����Ѵ�.
     *   - Primary Key�� ������ �����Ƿ�, mPKCIDs[]�� mPKCols[]�� �����Ѵ�.
     *
     * Column : mColCnt, mCIDs[], mBCols[], mACols[]
     */
    (void)aMeta->searchRemoteTable(&sItem, aXLog->mTableOID);
    
    IDU_FIT_POINT_RAISE( "rpnComm::recvUpdateA7::Erratic::rpERR_ABORT_RECEIVER_NOT_FOUND_TABLE",
                         ERR_NOT_FOUND_TABLE );
    IDE_TEST_RAISE(sItem == NULL, ERR_NOT_FOUND_TABLE);

    /* Recv Update Value repeatedly */
    for(i = 0; i < aXLog->mColCnt; i++)
    {
        IDE_TEST( recvCIDA7( aExitFlag,
                             aProtocolContext,
                             &aXLog->mCIDs[i],
                             aTimeOutSec )
                  != IDE_SUCCESS );

        aXLog->mCIDs[i] = sItem->mMapColID[aXLog->mCIDs[i]];

        if( aXLog->mCIDs[i] != RP_INVALID_COLUMN_ID )
        {
            IDE_TEST(recvValueA7(aAllocator,
                                 aExitFlag,
                                 aProtocolContext,
                                 &aXLog->mMemory,
                                 &aXLog->mBCols[i],
                                 aTimeOutSec)
                     != IDE_SUCCESS);
            IDE_TEST(recvValueA7(aAllocator,
                                 aExitFlag,
                                 aProtocolContext,
                                 &aXLog->mMemory,
                                 &aXLog->mACols[i],
                                 aTimeOutSec)
                     != IDE_SUCCESS);
        }
        else
        {
            IDE_TEST(recvValueA7(aAllocator,
                                 aExitFlag,
                                 aProtocolContext,
                                 NULL, /* aMemory */
                                 &sDummy,
                                 aTimeOutSec)
                     != IDE_SUCCESS);
            if(sDummy.value != NULL)
            {
                (void)iduMemMgr::free((void *)sDummy.value, aAllocator);
            }

            IDE_TEST(recvValueA7(aAllocator,
                                 aExitFlag,
                                 aProtocolContext,
                                 NULL, /* aMemory */
                                 &sDummy,
                                 aTimeOutSec)
                     != IDE_SUCCESS);
            if(sDummy.value != NULL)
            {
                (void)iduMemMgr::free((void *)sDummy.value, aAllocator);
            }

            i--;
            aXLog->mColCnt--;
        }
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION(ERR_NOT_FOUND_TABLE);
    {
        IDE_SET(ideSetErrorCode(rpERR_ABORT_RECEIVER_NOT_FOUND_TABLE,
                                aXLog->mSN,
                                aXLog->mTableOID));
    }
    IDE_EXCEPTION( ERR_MEMORY_ALLOC );
    {
        IDE_ERRLOG( IDE_RP_0 );
        IDE_SET(ideSetErrorCode(rpERR_ABORT_MEMORY_ALLOC,
                                "rpnComm::recvUpdateA7",
                                "Column"));
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendDeleteA7( void               *aHBTResource,
                              cmiProtocolContext *aProtocolContext,
                              smTID               aTID,
                              smSN                aSN,
                              smSN                aSyncSN,
                              UInt                aImplSPDepth,
                              ULong               aTableOID,
                              UInt                aPKColCnt,
                              smiValue          * aPKCols,
                              rpValueLen        * aPKLen,
                              UInt                aColCnt,
                              UInt              * aCIDs,
                              smiValue          * aBCols,
                              smiChainedValue   * aBChainedCols, // PROJ-1705
                              rpValueLen        * aBMtdValueLen,
                              UInt              * aBChainedColsTotalLen )
{
    UInt i;
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType= RP_X_DELETE;

    /* Validate Parameters */
    IDE_DASSERT( aImplSPDepth <= SMI_STATEMENT_DEPTH_MAX );
    IDE_DASSERT( aPKColCnt <= QCI_MAX_KEY_COLUMN_COUNT );
    IDE_DASSERT( aPKCols != NULL );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 44, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Delete );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information     */
    CMI_WR4( aProtocolContext, &(aImplSPDepth) );
    CMI_WR8( aProtocolContext, &(aTableOID) );
    CMI_WR4( aProtocolContext, &(aPKColCnt) );
    CMI_WR4( aProtocolContext, &(aColCnt) );

    /* Send Primary Key Value */
    for(i = 0; i < aPKColCnt; i ++)
    {
        IDE_TEST( sendPKValueForA7( aHBTResource,
                                    aProtocolContext,
                                    &aPKCols[i],
                                    aPKLen[i] )
                  != IDE_SUCCESS );
    }

    /* Send old values */
    for ( i = 0; i < aColCnt; i++ )
    {
        IDE_TEST( sendCIDForA7( aHBTResource,
                                aProtocolContext,
                                aCIDs[i] )
                  != IDE_SUCCESS );

        /*
         * �޸� ���̺��� �߻��� update�� �м� ����� BCols�� ����ִ�.
         * mAllocMethod�� �ʱⰪ�� SMI_NON_ALLOCED
         */
        if ( ( aBChainedCols[aCIDs[i]].mAllocMethod == SMI_NON_ALLOCED ) &&
             ( aBMtdValueLen[aCIDs[i]].lengthSize == 0 ) )
        {
            IDE_TEST( sendValueForA7( aHBTResource,
                                      aProtocolContext,
                                      &aBCols[aCIDs[i]],
                                      aBMtdValueLen[aCIDs[i]])
                      != IDE_SUCCESS );
        }
        // ��ũ ���̺��� �߻��� update�� �м� ����� BChainedCols�� ����ִ�.
        else
        {
            IDE_TEST( sendChainedValueForA7( aHBTResource,
                                             aProtocolContext,
                                             &aBChainedCols[aCIDs[i]],
                                             aBMtdValueLen[aCIDs[i]],
                                             aBChainedColsTotalLen[aCIDs[i]] )
                     != IDE_SUCCESS );
        }
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvDeleteA7( iduMemAllocator     * aAllocator,
                              idBool              * aExitFlag,
                              cmiProtocolContext  * aProtocolContext,
                              rpdMeta             * aMeta,
                              rpdXLog             * aXLog,
                              ULong                 aTimeOutSec )
{
    UInt i;
    UInt * sType = (UInt*)&(aXLog->mType);

    rpdMetaItem    * sItem = NULL;
    smiValue         sDummy;

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Argument Savepoint Name */
    CMI_RD4( aProtocolContext, &(aXLog->mImplSPDepth) );
    CMI_RD8( aProtocolContext, &(aXLog->mTableOID) );
    CMI_RD4( aProtocolContext, &(aXLog->mPKColCnt) );
    CMI_RD4( aProtocolContext, &(aXLog->mColCnt) );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mPKColCnt * ID_SIZEOF( smiValue ),
                                          (void **)&( aXLog->mPKCols ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    if ( aXLog->mColCnt > 0 )
    {
        IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mColCnt * ID_SIZEOF( UInt ),
                                              (void **)&( aXLog->mCIDs ) )
                        != IDE_SUCCESS, ERR_MEMORY_ALLOC );

        IDE_TEST_RAISE( aXLog->mMemory.alloc(  aXLog->mColCnt * ID_SIZEOF( smiValue ),
                                               (void **)&( aXLog->mBCols ) )
                        != IDE_SUCCESS, ERR_MEMORY_ALLOC );
    }
    else
    {
        /* nothing to do */
    }

    /* Recv PK Value repeatedly */
    for( i = 0; i < aXLog->mPKColCnt; i ++ )
    {
        IDE_TEST(recvValueA7(aAllocator,
                             aExitFlag,
                             aProtocolContext,
                             &aXLog->mMemory,
                             &aXLog->mPKCols[i],
                             aTimeOutSec)
                 != IDE_SUCCESS);
    }

    /* PROJ-1442 Replication Online �� DDL ���
     * XLog ���� Column ������ ��迭�Ѵ�.
     *   - mPKColCnt�� ������ �����Ƿ�, �����Ѵ�.
     *   - Primary Key�� ������ �����Ƿ�, mPKCIDs[]�� mPKCols[]�� �����Ѵ�.
     *
     * Column : mColCnt, mCIDs[], mBCols[], mACols[]
     */
    (void)aMeta->searchRemoteTable(&sItem, aXLog->mTableOID);

    IDU_FIT_POINT_RAISE( "rpnComm::recvDeleteA7::Erratic::rpERR_ABORT_RECEIVER_NOT_FOUND_TABLE",
                         ERR_NOT_FOUND_TABLE );
    IDE_TEST_RAISE(sItem == NULL, ERR_NOT_FOUND_TABLE);

    /* Receive old values */
    for ( i = 0; i < aXLog->mColCnt; i++ )
    {
        IDE_TEST( recvCIDA7( aExitFlag,
                             aProtocolContext,
                             &aXLog->mCIDs[i],
                             aTimeOutSec )
                  != IDE_SUCCESS );

        aXLog->mCIDs[i] = sItem->mMapColID[aXLog->mCIDs[i]];

        if ( aXLog->mCIDs[i] != RP_INVALID_COLUMN_ID )
        {
            IDE_TEST( recvValueA7( aAllocator,
                                   aExitFlag,
                                   aProtocolContext,
                                   &aXLog->mMemory,
                                   &aXLog->mBCols[i],
                                   aTimeOutSec )
                      != IDE_SUCCESS);
        }
        else
        {
            IDE_TEST( recvValueA7( aAllocator,
                                   aExitFlag,
                                   aProtocolContext,
                                   NULL, /* aMemory */
                                   &sDummy,
                                   aTimeOutSec)
                      != IDE_SUCCESS);
            if ( sDummy.value != NULL )
            {
                (void)iduMemMgr::free( (void *)sDummy.value, aAllocator );
            }

            i--;
            aXLog->mColCnt--;
        }
    }

    /* PROJ-1442 Replication Online �� DDL ���
     * XLog ���� Column ������ ��迭�Ѵ�.
     *   - mPKColCnt�� ������ �����Ƿ�, �����Ѵ�.
     *   - Primary Key�� ������ �����Ƿ�, mPKCIDs[]�� mPKCols[]�� �����Ѵ�.
     */

    return IDE_SUCCESS;

    IDE_EXCEPTION(ERR_NOT_FOUND_TABLE);
    {
        IDE_SET(ideSetErrorCode(rpERR_ABORT_RECEIVER_NOT_FOUND_TABLE,
                                aXLog->mSN,
                                aXLog->mTableOID));
    }
    IDE_EXCEPTION( ERR_MEMORY_ALLOC );
    {
        IDE_ERRLOG( IDE_RP_0 );
        IDE_SET(ideSetErrorCode(rpERR_ABORT_MEMORY_ALLOC,
                                "rpnComm::recvDeleteA7",
                                "Column"));
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendCIDForA7( void               * aHBTResource,
                              cmiProtocolContext * aProtocolContext,
                              UInt                 aCID )
{
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    /* Validate Parameters */
    IDE_DASSERT( aCID < QCI_MAX_COLUMN_COUNT );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 4, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, UIntID );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(aCID) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvCIDA7( idBool              * aExitFlag,
                           cmiProtocolContext  * aProtocolContext,
                           UInt                * aCID,
                           ULong                 aTimeoutSec)
{
    UChar sOpCode;

    if ( CMI_IS_READ_ALL( aProtocolContext ) == ID_TRUE )
    {
        IDE_TEST( readCmBlock( aProtocolContext, aExitFlag, NULL /* TimeoutFlag */, aTimeoutSec )
                  != IDE_SUCCESS );
    }

    CMI_RD1( aProtocolContext, sOpCode );

    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, UIntID ),
                    ERR_CHECK_OPERATION_TYPE );

    /* Get Argument */
    CMI_RD4( aProtocolContext, aCID );

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendTxAckA7( cmiProtocolContext * aProtocolContext,
                             smTID                aTID,
                             smSN                 aSN )
{
    UChar sOpCode;

    IDE_TEST( checkAndFlush( NULL,
                             aProtocolContext, 
                             1 + 12, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, TxAck );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvTxAckForA7( idBool             * aExitFlag,
                                cmiProtocolContext * aProtocolContext,
                                smTID              * aTID,
                                smSN               * aSN,
                                ULong                aTimeoutSec )
{
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    if ( CMI_IS_READ_ALL( aProtocolContext ) == ID_TRUE )
    {
        IDE_TEST( readCmBlock( aProtocolContext, aExitFlag, NULL /* TimeoutFlag */, aTimeoutSec )
                  != IDE_SUCCESS );
    }

    /* Check Operation Type */
    CMI_RD1( aProtocolContext, sOpCode );
    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, TxAck ),
                    ERR_CHECK_OPERATION_TYPE );
    /* Get Argument */
    CMI_RD4( aProtocolContext, aTID );
    CMI_RD8( aProtocolContext, aSN );

    return IDE_SUCCESS;

    IDE_EXCEPTION(ERR_CHECK_OPERATION_TYPE);
    {
        IDE_SET(ideSetErrorCode(rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                sOpCode));
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendValueForA7( void               * aHBTResource,
                                cmiProtocolContext * aProtocolContext,
                                smiValue           * aValue,
                                rpValueLen           aValueLen )
{
    UChar sOneByteLenValue;
    UInt sRemainSpaceInCmBlock;
    UInt sRemainDataOfValue;
    UInt sLength;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    /* Validate Parameters */
    IDE_DASSERT( aValue != NULL );

    /* check : OpCode (1) + value length(4)
     * value�� �� ������ ���̴��� ���������� �Ǵ��ϱ� ���ؼ��� length������ �� �� �־���ϹǷ�
     * �� ������ ������ �� ������ �׳� �� cmBlock�� ����Ѵ�
     */
    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext,
                             1 + 4 + 2,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Value );
    CMI_WR1( aProtocolContext, sOpCode );

    /* Set Communication Argument : mtdValueLength + value */

    /* Send mtdValueLength : �� ���� value�� �ϳ��� �����ͷ� ����Ͽ� ���۵Ǿ���Ѵ�.
     * ���� ������ ó�� ���� CMI_WCP�� �׳� �����ϵ��� �Ѵ�.
     * memoryTable, non-divisible value �Ǵ� null value�� ���, rpValueLen������ �ʱⰪ�̴�.
     */
    if ( aValueLen.lengthSize > 0 )
    {
        /* Value length Set */
        sLength = (UInt)( aValueLen.lengthSize + aValueLen.lengthValue );
        CMI_WR4( aProtocolContext, &sLength );

        /* UShort�� UChar �����ͷ� 1����Ʈ�� ������ ���, endian������ �߻��Ѵ�.
         * UChar ������ ��� ������ ��ġ�����Ѵ�.
         */
        if ( aValueLen.lengthSize == 1 )
        {
            sOneByteLenValue = (UChar)aValueLen.lengthValue;
            CMI_WCP( aProtocolContext, (UChar*)&sOneByteLenValue, 1 );
        }
        else
        {
            // BUGBUG  lengthValue ������ 1 �Ǵ� 2 byte �̿��� ũ�Ⱑ �����ұ�?
            CMI_WCP( aProtocolContext, (UChar*)&aValueLen.lengthValue, aValueLen.lengthSize );
        }
    }
    else
    {
        /* Value length Set */
        sLength = aValue->length;
        CMI_WR4( aProtocolContext, &sLength );
    }

    sRemainDataOfValue = aValue->length;
    sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );

    while ( sRemainDataOfValue > 0 )
    {
        if ( sRemainSpaceInCmBlock == 0 )
        {
            IDU_FIT_POINT( "rpnComm::sendValue::cmiSend::ERR_SEND" );
            IDE_TEST( sendCmBlock( aHBTResource,
                                   aProtocolContext,
                                   ID_FALSE )
                      != IDE_SUCCESS );
            sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
        }
        else
        {
            if ( sRemainSpaceInCmBlock >= sRemainDataOfValue )
            {
                CMI_WCP( aProtocolContext,
                         (UChar *)aValue->value + ( aValue->length - sRemainDataOfValue ),
                         sRemainDataOfValue );
                sRemainDataOfValue = 0;
            }
            else
            {
                CMI_WCP( aProtocolContext,
                         (UChar *)aValue->value + ( aValue->length - sRemainDataOfValue ),
                         sRemainSpaceInCmBlock );
                sRemainDataOfValue = sRemainDataOfValue - sRemainSpaceInCmBlock;
            }
            sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
        }
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendPKValueForA7( void               * aHBTResource,
                                  cmiProtocolContext * aProtocolContext,
                                  smiValue           * aValue,
                                  rpValueLen           aPKLen )
{
    UChar sOneByteLenValue;
    UInt sRemainSpaceInCmBlock;
    UInt sRemainDataOfValue;
    UInt sLength;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    /* Validate Parameters */
    IDE_DASSERT( aValue != NULL );

    /*
     * PROJ-1705�� ����,
     * divisible data type�� ���, null value�̸�, �տ� mtdLength��ŭ�� ���̸� length�� ������,
     * value���� �� ���� ��ŭ�� �����͸� �����ϰ� ������ �����Ƿ�,
     * (rpLenSize ���� �� ���� ������ �ִ�.) �ٸ� �Լ������� ������ ������ ASSERT�� �����Ͽ�����,
     * PK Value�� null�� �� �� �����Ƿ� ������Ŵ.
     */
    IDE_DASSERT( (aValue->value != NULL) || (aValue->length == 0) );

    /* check : OpCode (1) + value length(4)
     * value�� �� ������ ���̴��� ���������� �Ǵ��ϱ� ���ؼ��� length������ �� �� �־���ϹǷ�
     * �� ������ ������ �� ������ �׳� �� cmBlock�� ����Ѵ�
     */
    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext,
                             1 + 4 + 2,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Value );
    CMI_WR1( aProtocolContext, sOpCode );

    /* Set Communication Argument */

    /* Send mtdValueLength : �� ���� value�� �ϳ��� �����ͷ� ����Ͽ� ���۵Ǿ���Ѵ�.
     * ���� ������ ó�� ���� CMI_WCP�� �׳� �����ϵ��� �Ѵ�.
     * memoryTable, non-divisible value �Ǵ� null value�� ���, rpValueLen������ �ʱⰪ�̴�.
     */
    if( aPKLen.lengthSize > 0 )
    {
        /* Value length Set */
        sLength = (UInt)( aPKLen.lengthSize + aPKLen.lengthValue );
        CMI_WR4( aProtocolContext, &sLength );

        // UShort�� UChar �����ͷ� 1����Ʈ�� ������ ���, endian������ �߻��Ѵ�.
        // UChar ������ ��� ������ ��ġ�����Ѵ�.
        if( aPKLen.lengthSize == 1 )
        {
            sOneByteLenValue = (UChar)aPKLen.lengthValue;
            CMI_WCP( aProtocolContext, (UChar*)&sOneByteLenValue, 1 );
        }
        else
        {
            // BUGBUG  lengthValue ������ 1 �Ǵ� 2 byte �̿��� ũ�Ⱑ �����ұ�?
            CMI_WCP( aProtocolContext, (UChar*)&aPKLen.lengthValue, aPKLen.lengthSize);
        }
    }
    else
    {
        /* Value length Set */
        sLength = aValue->length;
        CMI_WR4( aProtocolContext, &sLength );
    }

    sRemainDataOfValue = aValue->length;
    sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
    while ( sRemainDataOfValue > 0 )
    {
        if ( sRemainSpaceInCmBlock == 0 )
        {
            IDU_FIT_POINT( "rpnComm::sendPKValue::cmiSend::ERR_SEND" );
            IDE_TEST( sendCmBlock( aHBTResource,
                                   aProtocolContext,
                                   ID_FALSE )
                      != IDE_SUCCESS );
            sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
        }
        else
        {
            if ( sRemainSpaceInCmBlock >= sRemainDataOfValue )
            {
                CMI_WCP( aProtocolContext,
                         (UChar *)aValue->value + ( aValue->length - sRemainDataOfValue ),
                         sRemainDataOfValue );
                sRemainDataOfValue = 0;
            }
            else
            {
                CMI_WCP( aProtocolContext,
                         (UChar *)aValue->value + ( aValue->length - sRemainDataOfValue ),
                         sRemainSpaceInCmBlock );
                sRemainDataOfValue = sRemainDataOfValue - sRemainSpaceInCmBlock;
            }
            sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
        }
    }
    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendChainedValueForA7( void               * aHBTResource,
                                       cmiProtocolContext * aProtocolContext,
                                       smiChainedValue    * aChainedValue,
                                       rpValueLen           aBMtdValueLen,
                                       UInt                 aChainedValueLen )
{
    smiChainedValue * sChainedValue = NULL;
    UChar sOneByteLenValue;
    UInt sRemainSpaceInCmBlock;
    UInt sRemainDataOfValue;
    UInt sLength;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    /* Validate Parameters */
    IDE_DASSERT(aChainedValue != NULL);

    sChainedValue = aChainedValue;

    /* check : OpCode (1) + value length(4)
     * value�� �� ������ ���̴��� ���������� �Ǵ��ϱ� ���ؼ��� length������ �� �� �־���ϹǷ�
     * �� ������ ������ �� ������ �׳� �� cmBlock�� ����Ѵ�
     */
    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext,
                             1 + 4 + 2,
                             ID_TRUE )
              != IDE_SUCCESS );
    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Value );
    CMI_WR1( aProtocolContext, sOpCode );

    /* Set Communication Argument : mtdValueLength + value */

    /* Send mtdValueLength : �� ���� value�� �ϳ��� �����ͷ� ����Ͽ� ���۵Ǿ���Ѵ�.
     * ���� ������ ó�� ���� CMI_WCP�� �׳� �����ϵ��� �Ѵ�.
     * memoryTable, non-divisible value �Ǵ� null value�� ���, rpValueLen������ �ʱⰪ�̴�.
     */

    if ( aBMtdValueLen.lengthSize > 0 )
    {
        /* Value length Set */
        sLength = (UInt)( aBMtdValueLen.lengthSize + aBMtdValueLen.lengthValue );
        CMI_WR4( aProtocolContext, &sLength );

        /*
         * UShort�� UChar �����ͷ� 1����Ʈ�� ������ ���, endian������ �߻��Ѵ�.
         * UChar ������ ��� ������ ��ġ�����Ѵ�.
         */
        if ( aBMtdValueLen.lengthSize == 1 )
        {
            sOneByteLenValue = (UChar)aBMtdValueLen.lengthValue;
            CMI_WCP( aProtocolContext, (UChar*)&sOneByteLenValue, 1 );
        }
        else
        {
            // BUGBUG  lengthValue ������ 1 �Ǵ� 2 byte �̿��� ũ�Ⱑ �����ұ�?
            CMI_WCP( aProtocolContext, (UChar*)&aBMtdValueLen.lengthValue, aBMtdValueLen.lengthSize );
        }
    }
    else
    {
        /* Value length Set */
        sLength = aChainedValueLen; /* BUG-33722 */
        CMI_WR4( aProtocolContext, &sLength );
    }

    sRemainDataOfValue = sChainedValue->mColumn.length;
    sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
    while ( sRemainDataOfValue > 0 )
    {
        /* mColumn.length�� 0���� ū��, value�� NULL�� �� �ֳ�? */
        IDE_ASSERT( sChainedValue->mColumn.value != NULL );

        if ( sRemainSpaceInCmBlock == 0 )
        {
            IDU_FIT_POINT( "rpnComm::sendChainedValue::cmiSend::ERR_SEND" );
            IDE_TEST( sendCmBlock( aHBTResource,
                                   aProtocolContext,
                                   ID_FALSE )
                      != IDE_SUCCESS );
            sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
        }
        else
        {
            if ( sRemainSpaceInCmBlock >= sRemainDataOfValue )
            {
                CMI_WCP( aProtocolContext,
                         (UChar *)sChainedValue->mColumn.value + ( sChainedValue->mColumn.length - sRemainDataOfValue ),
                         sRemainDataOfValue );
                sChainedValue = sChainedValue->mLink;
                if ( sChainedValue == NULL )
                {
                    break;
                }
                else
                {
                    sRemainDataOfValue = sChainedValue->mColumn.length;
                }
            }
            else
            {
                CMI_WCP( aProtocolContext,
                         (UChar *)sChainedValue->mColumn.value + ( sChainedValue->mColumn.length - sRemainDataOfValue ),
                         sRemainSpaceInCmBlock );
                sRemainDataOfValue = sRemainDataOfValue - sRemainSpaceInCmBlock;
            }
            sRemainSpaceInCmBlock = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
        }
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvValueA7( iduMemAllocator     * aAllocator,
                             idBool              * aExitFlag,
                             cmiProtocolContext  * aProtocolContext,
                             iduMemory           * aMemory,
                             smiValue            * aValue,
                             ULong                 aTimeOutSec )
{
    UChar   sOpCode               = CMI_PROTOCOL_OPERATION( RP, MAX );
    UInt    sRemainDataInCmBlock  = 0;
    UInt    sRemainDataOfValue    = 0;
    idBool  sIsAllocByiduMemMgr   = ID_FALSE;

    aValue->value = NULL;

    if ( CMI_IS_READ_ALL( aProtocolContext ) == ID_TRUE )
    {
        IDE_TEST( readCmBlock( aProtocolContext, aExitFlag, NULL /* TimeoutFlag */, aTimeOutSec )
                  != IDE_SUCCESS );
    }

    CMI_RD1( aProtocolContext, sOpCode );
    IDE_TEST_RAISE( sOpCode != CMI_PROTOCOL_OPERATION( RP, Value ),
                    ERR_CHECK_OPERATION_TYPE );

    CMI_RD4( aProtocolContext, &(aValue->length) );

    /* NOTICE!!!!!
     * ���� Value�� ���� ���� �����ϴ� �޸� ������ ���⼭ �Ҵ�޴´�.
     * ����, �� XLog�� ����� ��(ReceiverApply::execXLog)����
     * ���� XLog�� ����Ͽ�����, ���⼭ �Ҵ����� �޸� ������
     * ������ �־�� �Ѵ�.
     * �̷��� Value�� ���� �޸𸮸� �Ҵ��ϴ� Protocol��
     * Insert, Update, Delete, LOB cursor open�� �ִ�.
     */
    if( aValue->length > 0 )
    {
        /* NULL value�� ���۵Ǵ� ��쿡�� length�� 0���� �Ѿ����
         * �ǹǷ�, �� ��쿡�� �޸� �Ҵ��� ���� �ʵ��� �Ѵ�. */
        IDU_FIT_POINT_RAISE( "rpnComm::recvValueA7::malloc::Value", 
                              ERR_MEMORY_ALLOC_VALUE );
        if ( aMemory != NULL )
        {
            IDE_TEST_RAISE( aMemory->alloc( aValue->length,
                                            (void **)&aValue->value )
                            != IDE_SUCCESS, ERR_MEMORY_ALLOC_VALUE );
        }
        else
        {
            IDE_TEST_RAISE( iduMemMgr::malloc( IDU_MEM_RP_RPN,
                                               aValue->length,
                                               (void **)&aValue->value,
                                               IDU_MEM_IMMEDIATE,
                                               aAllocator )
                            != IDE_SUCCESS, ERR_MEMORY_ALLOC_VALUE );
            sIsAllocByiduMemMgr = ID_TRUE;
        }

        sRemainDataOfValue = aValue->length;
        sRemainDataInCmBlock = CMI_REMAIN_DATA_IN_READ_BLOCK( aProtocolContext );
        while ( sRemainDataOfValue > 0 )
        {
            if ( sRemainDataInCmBlock == 0 )
            {
                IDE_TEST( readCmBlock( aProtocolContext, aExitFlag, NULL /* TimeoutFlag */, aTimeOutSec )
                          != IDE_SUCCESS );
                sRemainDataInCmBlock = CMI_REMAIN_DATA_IN_READ_BLOCK( aProtocolContext );
            }
            else
            {
                if ( sRemainDataInCmBlock >= sRemainDataOfValue )
                {
                    CMI_RCP( aProtocolContext,
                             (UChar *)aValue->value + ( aValue->length - sRemainDataOfValue ),
                             sRemainDataOfValue );
                    sRemainDataOfValue = 0;
                }
                else
                {
                    CMI_RCP( aProtocolContext,
                             (UChar *)aValue->value + ( aValue->length - sRemainDataOfValue ),
                             sRemainDataInCmBlock );
                    sRemainDataOfValue = sRemainDataOfValue - sRemainDataInCmBlock;
                }
                sRemainDataInCmBlock = CMI_REMAIN_DATA_IN_READ_BLOCK( aProtocolContext );
            }
        }
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_ERRLOG(IDE_RP_0);
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG(IDE_RP_0);
    }
    IDE_EXCEPTION(ERR_MEMORY_ALLOC_VALUE);
    {
        IDE_ERRLOG(IDE_RP_0);
        IDE_SET(ideSetErrorCode(rpERR_ABORT_MEMORY_ALLOC,
                                "rpnComm::recvValueA7",
                                "aValue->value"));
        IDE_ERRLOG(IDE_RP_0);
    }
    IDE_EXCEPTION_END;

    if ( sIsAllocByiduMemMgr == ID_TRUE )
    {
        (void)iduMemMgr::free((void *)aValue->value, aAllocator);
        aValue->value = NULL;
    }
    else
    {
        /* Nothing to do */
    }

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendStopA7( void               * aHBTResource,
                            cmiProtocolContext * aProtocolContext,
                            smTID                aTID,
                            smSN                 aSN,
                            smSN                 aSyncSN,
                            smSN                 aRestartSN )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 32, 
                             ID_TRUE )
              != IDE_SUCCESS );

    sType = RP_X_REPL_STOP;

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Stop );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    CMI_WR8( aProtocolContext, &(aRestartSN) );

    IDU_FIT_POINT( "rpnComm::sendStop::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvStopA7( cmiProtocolContext  * aProtocolContext,
                            rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mRestartSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendKeepAliveA7( void               * aHBTResource,
                                 cmiProtocolContext * aProtocolContext,
                                 smTID                aTID,
                                 smSN                 aSN,
                                 smSN                 aSyncSN,
                                 smSN                 aRestartSN )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_KEEP_ALIVE;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 32, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, KeepAlive );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    CMI_WR8( aProtocolContext, &(aRestartSN) );

    IDU_FIT_POINT( "rpnComm::sendKeepAlive::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvKeepAliveA7( cmiProtocolContext  * aProtocolContext,
                                 rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mRestartSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendFlushA7( void               * /*aHBTResource*/,
                             cmiProtocolContext * aProtocolContext,
                             smTID                aTID,
                             smSN                 aSN,
                             smSN                 aSyncSN,
                             UInt                 aFlushOption)
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_FLUSH;

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Flush );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    CMI_WR4( aProtocolContext, &(aFlushOption) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvFlushA7( cmiProtocolContext  * aProtocolContext,
                             rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Flush Option */
    CMI_RD4( aProtocolContext, &(aXLog->mFlushOption) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendAckA7( cmiProtocolContext * aProtocolContext,
                           rpXLogAck            aAck )
{
    UInt i = 0;
    UChar sOpCode;

    IDE_TEST( checkAndFlush( NULL,
                             aProtocolContext, 
                             1 + 52, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Ack );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(aAck.mAckType) );
    CMI_WR4( aProtocolContext, &(aAck.mAbortTxCount) );
    CMI_WR4( aProtocolContext, &(aAck.mClearTxCount) );
    CMI_WR8( aProtocolContext, &(aAck.mRestartSN) );
    CMI_WR8( aProtocolContext, &(aAck.mLastCommitSN) );
    CMI_WR8( aProtocolContext, &(aAck.mLastArrivedSN) );
    CMI_WR8( aProtocolContext, &(aAck.mLastProcessedSN) );
    CMI_WR8( aProtocolContext, &(aAck.mFlushSN) );

    IDU_FIT_POINT( "1.TASK-2004@rpnComm::sendAckA7" );

    for(i = 0; i < aAck.mAbortTxCount; i ++)
    {
        IDE_TEST( sendTxAckA7( aProtocolContext,
                               aAck.mAbortTxList[i].mTID,
                               aAck.mAbortTxList[i].mSN )
                  != IDE_SUCCESS );
    }

    for(i = 0; i < aAck.mClearTxCount; i ++)
    {
        IDE_TEST( sendTxAckA7( aProtocolContext,
                               aAck.mClearTxList[i].mTID,
                               aAck.mClearTxList[i].mSN )
                  != IDE_SUCCESS );
    }

    IDU_FIT_POINT( "rpnComm::sendAckA7::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( NULL,    /* aHBTResource */
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvAckA7( iduMemAllocator    * /*aAllocator*/,
                           cmiProtocolContext * aProtocolContext,
                           idBool             * aExitFlag,
                           rpXLogAck          * aAck,
                           ULong                aTimeoutSec,
                           idBool             * aIsTimeoutSec )
{
    ULong i;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    *aIsTimeoutSec = ID_FALSE;

    if ( CMI_IS_READ_ALL( aProtocolContext ) == ID_TRUE )
    {
        IDE_TEST( readCmBlock( aProtocolContext, aExitFlag, aIsTimeoutSec, aTimeoutSec )
                  != IDE_SUCCESS );
    }

    IDU_FIT_POINT_RAISE( "1.TASK-2004@rpnComm::recvAck", 
                          ERR_EXIT );

    if( *aIsTimeoutSec == ID_FALSE )
    {
        /* Check Operation Type */
        CMI_RD1( aProtocolContext, sOpCode );

        IDE_TEST_RAISE( ( sOpCode != CMI_PROTOCOL_OPERATION( RP, Ack ) ) &&
                        ( sOpCode != CMI_PROTOCOL_OPERATION( RP, AckEager ) ),
                        ERR_CHECK_OPERATION_TYPE );
        /* Get argument */
        CMI_RD4( aProtocolContext, &(aAck->mAckType) );
        CMI_RD4( aProtocolContext, &(aAck->mAbortTxCount) );
        CMI_RD4( aProtocolContext, &(aAck->mClearTxCount) );
        CMI_RD8( aProtocolContext, &(aAck->mRestartSN) );
        CMI_RD8( aProtocolContext, &(aAck->mLastCommitSN) );
        CMI_RD8( aProtocolContext, &(aAck->mLastArrivedSN) );
        CMI_RD8( aProtocolContext, &(aAck->mLastProcessedSN) );
        CMI_RD8( aProtocolContext, &(aAck->mFlushSN) );

        if ( sOpCode == CMI_PROTOCOL_OPERATION( RP, AckEager ) )
        {
            CMI_RD4( aProtocolContext, &(aAck->mTID) );
        }
        else
        {
            aAck->mTID = SM_NULL_TID;
        }

        /* Get Abort Tx List */
        for(i = 0; i < aAck->mAbortTxCount; i++)
        {
            IDE_TEST(recvTxAckForA7(aExitFlag,
                                    aProtocolContext,
                                    &(aAck->mAbortTxList[i].mTID),
                                    &(aAck->mAbortTxList[i].mSN),
                                    aTimeoutSec)
                     != IDE_SUCCESS);
        }

        /* Get Clear Tx List */
        for(i = 0; i < aAck->mClearTxCount; i++)
        {
            IDE_TEST(recvTxAckForA7(aExitFlag,
                                    aProtocolContext,
                                    &(aAck->mClearTxList[i].mTID),
                                    &(aAck->mClearTxList[i].mSN),
                                    aTimeoutSec)
                     != IDE_SUCCESS);
        }
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_CHECK_OPERATION_TYPE );
    {
        IDE_SET( ideSetErrorCode( rpERR_ABORT_RP_WRONG_OPERATION_TYPE,
                                  sOpCode ) );
        IDE_ERRLOG( IDE_RP_0 );
    }
    IDE_EXCEPTION(ERR_EXIT);
    {
        IDE_SET(ideSetErrorCode(rpERR_IGNORE_EXIT_FLAG_SET));
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendLobCursorOpenA7( void               * aHBTResource,
                                     cmiProtocolContext * aProtocolContext,
                                     smTID                aTID,
                                     smSN                 aSN,
                                     smSN                 aSyncSN,
                                     ULong                aTableOID,
                                     ULong                aLobLocator,
                                     UInt                 aLobColumnID,
                                     UInt                 aPKColCnt,
                                     smiValue           * aPKCols,
                                     rpValueLen         * aPKLen )
{
    UInt i;
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_LOB_CURSOR_OPEN;

    /* Validate Parameters */
    IDE_DASSERT(aLobColumnID < QCI_MAX_COLUMN_COUNT);
    IDE_DASSERT(aPKColCnt <= QCI_MAX_KEY_COLUMN_COUNT);
    IDE_DASSERT(aPKCols != NULL);

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 48, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, LobCursorOpen );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information */
    CMI_WR8( aProtocolContext, &(aTableOID) );
    CMI_WR8( aProtocolContext, &(aLobLocator) );
    CMI_WR4( aProtocolContext, &(aLobColumnID) );
    CMI_WR4( aProtocolContext, &(aPKColCnt) );

    /* Send Primary Key Value */
    for(i = 0; i < aPKColCnt; i ++)
    {
        IDE_TEST( sendPKValueForA7( aHBTResource,
                                    aProtocolContext,
                                    &aPKCols[i],
                                    aPKLen[i] )
                  != IDE_SUCCESS );
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvLobCursorOpenA7( iduMemAllocator     * aAllocator,
                                     idBool              * aExitFlag,
                                     cmiProtocolContext  * aProtocolContext,
                                     rpdMeta             * aMeta,  // BUG-20506
                                     rpdXLog             * aXLog,
                                     ULong                 aTimeOutSec )
{
    UInt                    i;
    rpdMetaItem            *sItem = NULL;
    UInt                    sLobColumnID;
    UInt * sType = (UInt*)&(aXLog->mType);

    IDE_ASSERT(aMeta != NULL);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Lob Information */
    CMI_RD8( aProtocolContext, &(aXLog->mTableOID) );
    CMI_RD8( aProtocolContext, &(aXLog->mLobPtr->mLobLocator) );
    CMI_RD4( aProtocolContext, &(sLobColumnID) );
    CMI_RD4( aProtocolContext, &(aXLog->mPKColCnt) );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mPKColCnt * ID_SIZEOF( smiValue ),
                                          (void **)&( aXLog->mPKCols ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    /* Recv PK Value repeatedly */
    for ( i = 0; i < aXLog->mPKColCnt; i ++ )
    {
        IDE_TEST( recvValueA7( aAllocator,
                               aExitFlag,
                               aProtocolContext,
                               &aXLog->mMemory,
                               &aXLog->mPKCols[i],
                               aTimeOutSec )
                  != IDE_SUCCESS );
    }

    /* PROJ-1442 Replication Online �� DDL ���
     * XLog ���� Column ������ ��迭�Ѵ�.
     *   - mPKColCnt�� ������ �����Ƿ�, �����Ѵ�.
     *   - Primary Key�� ������ �����Ƿ�, mPKCIDs[]�� mPKCols[]�� �����Ѵ�.
     *
     * LOB    : mLobColumnID
     */
    (void)aMeta->searchRemoteTable(&sItem, aXLog->mTableOID);

    IDE_TEST_RAISE(sItem == NULL, ERR_NOT_FOUND_TABLE);

    aXLog->mLobPtr->mLobColumnID = sItem->mMapColID[sLobColumnID];

    return IDE_SUCCESS;

    IDE_EXCEPTION(ERR_NOT_FOUND_TABLE);
    {
        IDE_SET(ideSetErrorCode(rpERR_ABORT_RECEIVER_NOT_FOUND_TABLE,
                                aXLog->mSN,
                                aXLog->mTableOID));
    }
    IDE_EXCEPTION( ERR_MEMORY_ALLOC );
    {
        IDE_ERRLOG( IDE_RP_0 );
        IDE_SET(ideSetErrorCode(rpERR_ABORT_MEMORY_ALLOC,
                                "rpnComm::recvLobCursorOpenA7",
                                "Column"));
    }

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendLobCursorCloseA7( void                * aHBTResource,
                                      cmiProtocolContext  * aProtocolContext,
                                      smTID                 aTID,
                                      smSN                  aSN,
                                      smSN                  aSyncSN,
                                      ULong                 aLobLocator )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_LOB_CURSOR_CLOSE;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 32, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, LobCursorClose );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information     */
    CMI_WR8( aProtocolContext, &(aLobLocator) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvLobCursorCloseA7( cmiProtocolContext  * aProtocolContext,
                                      rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Lob Information */
    CMI_RD8( aProtocolContext, &(aXLog->mLobPtr->mLobLocator) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendLobPrepare4WriteA7( void               * aHBTResource,
                                        cmiProtocolContext * aProtocolContext,
                                        smTID                aTID,
                                        smSN                 aSN,
                                        smSN                 aSyncSN,
                                        ULong                aLobLocator,
                                        UInt                 aLobOffset,
                                        UInt                 aLobOldSize,
                                        UInt                 aLobNewSize )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );

    sType = RP_X_LOB_PREPARE4WRITE;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 44, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, LobPrepare4Write );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information */
    CMI_WR8( aProtocolContext, &(aLobLocator) );
    CMI_WR4( aProtocolContext, &(aLobOffset) );
    CMI_WR4( aProtocolContext, &(aLobOldSize) );
    CMI_WR4( aProtocolContext, &(aLobNewSize) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvLobPrepare4WriteA7( cmiProtocolContext  * aProtocolContext,
                                        rpdXLog             * aXLog )
{
    UInt   * sType   = (UInt*)&(aXLog->mType);
    rpdLob * sLobPtr = aXLog->mLobPtr;

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Lob Information */
    CMI_RD8( aProtocolContext, &(sLobPtr->mLobLocator) );
    CMI_RD4( aProtocolContext, &(sLobPtr->mLobOffset) );
    CMI_RD4( aProtocolContext, &(sLobPtr->mLobOldSize) );
    CMI_RD4( aProtocolContext, &(sLobPtr->mLobNewSize) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendLobTrimA7( void               * aHBTResource,
                               cmiProtocolContext * aProtocolContext,
                               smTID                aTID,
                               smSN                 aSN,
                               smSN                 aSyncSN,
                               ULong                aLobLocator,
                               UInt                 aLobOffset )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_LOB_TRIM;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 36, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, LobTrim );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information */
    CMI_WR8( aProtocolContext, &(aLobLocator) );
    CMI_WR4( aProtocolContext, &(aLobOffset) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendLobPartialWriteA7( void               * aHBTResource,
                                       cmiProtocolContext * aProtocolContext,
                                       smTID                aTID,
                                       smSN                 aSN,
                                       smSN                 aSyncSN,
                                       ULong                aLobLocator,
                                       UInt                 aLobOffset,
                                       UInt                 aLobPieceLen,
                                       SChar              * aLobPiece )
{
    UInt  sType;
    UChar sOpCode;
    UInt  sRemainSize = 0;
    UInt  sSendSize = 0;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_LOB_PARTIAL_WRITE;

    /* ASSERT : Size must not be 0 */
    IDE_DASSERT( aLobPieceLen != 0 );

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             RP_BLOB_VALUSE_HEADER_SIZE_FOR_CM + aLobPieceLen,
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, LobPartialWrite );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information */
    CMI_WR8( aProtocolContext, &(aLobLocator) );
    CMI_WR4( aProtocolContext, &(aLobOffset) );
    CMI_WR4( aProtocolContext, &(aLobPieceLen) );

    sSendSize = aLobPieceLen;
    sRemainSize = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
    while( sSendSize > 0 )
    {
        if ( sRemainSize == 0 )
        {
            IDE_TEST( sendCmBlock( aHBTResource,
                                   aProtocolContext,
                                   ID_FALSE )
                      != IDE_SUCCESS );
        }
        else
        {
            if ( sRemainSize >= sSendSize )
            {
                CMI_WCP( aProtocolContext, 
                         (UChar *)aLobPiece + ( aLobPieceLen - sSendSize ), 
                         sSendSize );
                sSendSize = 0;
            }
            else
            {
                CMI_WCP( aProtocolContext, 
                         (UChar *)aLobPiece + ( aLobPieceLen - sSendSize ), 
                         sRemainSize );
                sSendSize = sSendSize - sRemainSize;
            }
        }
        sRemainSize = CMI_REMAIN_SPACE_IN_WRITE_BLOCK( aProtocolContext );
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvLobPartialWriteA7( iduMemAllocator     * aAllocator,
                                       idBool              * aExitFlag,
                                       cmiProtocolContext  * aProtocolContext,
                                       rpdXLog             * aXLog,
                                       ULong                 aTimeOutSec )
{
    UInt * sType = (UInt*)&(aXLog->mType);
    UInt   sRecvSize = 0;
    UInt   sRemainSize = 0;

    rpdLob * sLobPtr = aXLog->mLobPtr;

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Lob Information */
    CMI_RD8( aProtocolContext, &(sLobPtr->mLobLocator) );
    CMI_RD4( aProtocolContext, &(sLobPtr->mLobOffset) );
    CMI_RD4( aProtocolContext, &(sLobPtr->mLobPieceLen) );

    IDE_TEST_RAISE(iduMemMgr::malloc(IDU_MEM_RP_RPN,
                                     sLobPtr->mLobPieceLen,
                                     (void **)&(sLobPtr->mLobPiece),
                                     IDU_MEM_IMMEDIATE,
                                     aAllocator)
                   != IDE_SUCCESS, ERR_MEMORY_ALLOC_LOB_PIECE);

    sRecvSize = sLobPtr->mLobPieceLen;
    sRemainSize = CMI_REMAIN_DATA_IN_READ_BLOCK( aProtocolContext );
    while( sRecvSize > 0 )
    {
        if ( sRemainSize == 0 )
        {
            IDE_TEST( readCmBlock( aProtocolContext,
                                   aExitFlag,
                                   NULL,
                                   aTimeOutSec )
                      != IDE_SUCCESS );
        }
        else
        {
            if ( sRemainSize >= sRecvSize )
            {
                CMI_RCP( aProtocolContext, 
                         (UChar *)sLobPtr->mLobPiece + ( sLobPtr->mLobPieceLen - sRecvSize ), 
                         sRecvSize );
                sRecvSize = 0;
            }
            else
            {
                CMI_RCP( aProtocolContext, 
                         (UChar *)sLobPtr->mLobPiece + ( sLobPtr->mLobPieceLen - sRecvSize ), 
                         sRemainSize );
                sRecvSize = sRecvSize - sRemainSize;
            }
        }
        sRemainSize = CMI_REMAIN_DATA_IN_READ_BLOCK( aProtocolContext );
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION(ERR_MEMORY_ALLOC_LOB_PIECE);
    {
        IDE_ERRLOG(IDE_RP_0);
        IDE_SET(ideSetErrorCode(rpERR_ABORT_MEMORY_ALLOC,
                                "rpnComm::recvLobPartialWrite",
                                "aXLog->mLobPtr->mLobPiece"));
    }
    IDE_EXCEPTION_END;

    if(sLobPtr->mLobPiece != NULL)
    {
        (void)iduMemMgr::free(sLobPtr->mLobPiece, aAllocator);
        sLobPtr->mLobPiece = NULL;
    }

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendLobFinish2WriteA7( void               * aHBTResource,
                                       cmiProtocolContext * aProtocolContext,
                                       smTID                aTID,
                                       smSN                 aSN,
                                       smSN                 aSyncSN,
                                       ULong                aLobLocator )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_LOB_FINISH2WRITE;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 32, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, LobFinish2Write );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information */
    CMI_WR8( aProtocolContext, &(aLobLocator) );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvLobFinish2WriteA7( cmiProtocolContext  * aProtocolContext,
                                       rpdXLog             * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Lob Information */
    CMI_RD8( aProtocolContext, &(aXLog->mLobPtr->mLobLocator) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendHandshakeA7( void               * aHBTResource,
                                 cmiProtocolContext * aProtocolContext,
                                 smTID                aTID,
                                 smSN                 aSN,
                                 smSN                 aSyncSN )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_HANDSHAKE;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 24, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, Handshake );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );

    IDU_FIT_POINT( "rpnComm::sendHandshake::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvHandshakeA7( cmiProtocolContext * aProtocolContext,
                                 rpdXLog            * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendSyncPKBeginA7( void               * aHBTResource,
                                   cmiProtocolContext * aProtocolContext,
                                   smTID                aTID,
                                   smSN                 aSN,
                                   smSN                 aSyncSN )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_SYNC_PK_BEGIN;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 24, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, SyncPKBegin );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );

    IDU_FIT_POINT( "rpnComm::sendSyncPKBegin::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvSyncPKBeginA7( cmiProtocolContext * aProtocolContext,
                                   rpdXLog            * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendSyncPKA7( void               * aHBTResource,
                              cmiProtocolContext * aProtocolContext,
                              smTID                aTID,
                              smSN                 aSN,
                              smSN                 aSyncSN,
                              ULong                aTableOID,
                              UInt                 aPKColCnt,
                              smiValue           * aPKCols,
                              rpValueLen         * aPKLen )
{
    UInt i;
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_SYNC_PK;

    /* Validate Parameters */
    IDE_DASSERT(aPKColCnt <= QCI_MAX_KEY_COLUMN_COUNT);
    IDE_DASSERT(aPKCols != NULL);

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 36, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, SyncPK );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );
    /* Additional Information */
    CMI_WR8( aProtocolContext, &(aTableOID) );
    CMI_WR4( aProtocolContext, &(aPKColCnt) );

    /* Send Primary Key Value */
    for(i = 0; i < aPKColCnt; i ++)
    {
        IDE_TEST( sendPKValueForA7( aHBTResource,
                                    aProtocolContext,
                                    &aPKCols[i],
                                    aPKLen[i] )
                 != IDE_SUCCESS);
    }

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvSyncPKA7( idBool             * aExitFlag,
                              cmiProtocolContext * aProtocolContext,
                              rpdXLog            * aXLog,
                              ULong                aTimeOutSec )
{
    UInt i;
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );
    /* Get Argument Savepoint Name */
    CMI_RD8( aProtocolContext, &(aXLog->mTableOID) );
    CMI_RD4( aProtocolContext, &(aXLog->mPKColCnt) );

    IDE_TEST_RAISE( aXLog->mMemory.alloc( aXLog->mPKColCnt * ID_SIZEOF( smiValue ),
                                          (void **)&( aXLog->mPKCols ) )
                    != IDE_SUCCESS, ERR_MEMORY_ALLOC );

    /* Recv PK Value repeatedly */
    for( i = 0; i < aXLog->mPKColCnt; i ++ )
    {
        IDE_TEST( recvValueA7( NULL, /* PK column value �޸𸮸� iduMemMgr�� �Ҵ�޾ƾ��Ѵ�. */
                               aExitFlag,
                               aProtocolContext,
                               NULL, /* aMemory: Sender�� ������ ���̹Ƿ�, �����Ҵ� */
                               &aXLog->mPKCols[i],
                               aTimeOutSec )
                  != IDE_SUCCESS );
    }

    /* PROJ-1442 Replication Online �� DDL ���
     * XLog ���� Column ������ ��迭�Ѵ�.
     *   - mPKColCnt�� ������ �����Ƿ�, �����Ѵ�.
     *   - Primary Key�� ������ �����Ƿ�, mPKCIDs[]�� mPKCols[]�� �����Ѵ�.
     */

    return IDE_SUCCESS;

    IDE_EXCEPTION( ERR_MEMORY_ALLOC );
    {
        IDE_ERRLOG( IDE_RP_0 );
        IDE_SET(ideSetErrorCode(rpERR_ABORT_MEMORY_ALLOC,
                                "rpnComm::recvSyncPKA7",
                                "Column"));
    }
    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::sendSyncPKEndA7( void               * aHBTResource,
                                 cmiProtocolContext * aProtocolContext,
                                 smTID                aTID,
                                 smSN                 aSN,
                                 smSN                 aSyncSN )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_SYNC_PK_END;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 24, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, SyncPKEnd );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );

    IDU_FIT_POINT( "rpnComm::sendSyncPKEnd::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvSyncPKEndA7( cmiProtocolContext * aProtocolContext,
                                 rpdXLog            * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::sendFailbackEndA7( void               * aHBTResource,
                                   cmiProtocolContext * aProtocolContext,
                                   smTID                aTID,
                                   smSN                 aSN,
                                   smSN                 aSyncSN )
{
    UInt sType;
    UChar sOpCode;

    IDE_TEST( validateA7Protocol( aProtocolContext ) != IDE_SUCCESS );
    
    sType = RP_X_FAILBACK_END;

    IDE_TEST( checkAndFlush( aHBTResource,
                             aProtocolContext, 
                             1 + 24, 
                             ID_TRUE )
              != IDE_SUCCESS );

    /* Replication XLog Header Set */
    sOpCode = CMI_PROTOCOL_OPERATION( RP, FailbackEnd );
    CMI_WR1( aProtocolContext, sOpCode );
    CMI_WR4( aProtocolContext, &(sType) );
    CMI_WR4( aProtocolContext, &(aTID) );
    CMI_WR8( aProtocolContext, &(aSN) );
    CMI_WR8( aProtocolContext, &(aSyncSN) );

    IDU_FIT_POINT( "rpnComm::sendFailbackEnd::cmiSend::ERR_SEND" );
    IDE_TEST( sendCmBlock( aHBTResource,
                           aProtocolContext, 
                           ID_TRUE ) 
              != IDE_SUCCESS );

    return IDE_SUCCESS;

    IDE_EXCEPTION_END;

    return IDE_FAILURE;
}

IDE_RC rpnComm::recvFailbackEndA7( cmiProtocolContext * aProtocolContext,
                                   rpdXLog            * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );
    CMI_RD4( aProtocolContext, &(aXLog->mTID) );
    CMI_RD8( aProtocolContext, &(aXLog->mSN) );
    CMI_RD8( aProtocolContext, &(aXLog->mSyncSN) );

    return IDE_SUCCESS;
}

IDE_RC rpnComm::recvAckOnDML( cmiProtocolContext * aProtocolContext,
                              rpdXLog            * aXLog )
{
    UInt * sType = (UInt*)&(aXLog->mType);

    /* Get Argument XLog Hdr */
    CMI_RD4( aProtocolContext, sType );

    return IDE_SUCCESS;
}