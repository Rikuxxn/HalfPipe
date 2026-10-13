//=============================================================================
//
// 行列ユーティリティ処理 [MatrixUtility.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "MatrixUtility.h"
#include "MathUtility.h"


//*****************************************************************************
// 名前空間 MatrixUtil
//*****************************************************************************
namespace MatrixUtil
{
    //=========================================================================
    // 単位行列
    //=========================================================================
    D3DXMATRIX Identity(void)
    {
        D3DXMATRIX result;

        D3DXMatrixIdentity(&result);

        return result;
    }

    //=========================================================================
    // 平行移動
    //=========================================================================
    D3DXMATRIX Translation(float x, float y, float z)
    {
        D3DXMATRIX result;

        D3DXMatrixTranslation(&result, x, y, z);

        return result;
    }

    D3DXMATRIX Translation(const D3DXVECTOR3& position)
    {
        return Translation(position.x, position.y, position.z);
    }

    //=========================================================================
    // X軸回転
    //=========================================================================
    D3DXMATRIX RotationX(float angle)
    {
        D3DXMATRIX result;

        D3DXMatrixRotationX(&result, angle);

        return result;
    }

    //=========================================================================
    // Y軸回転
    //=========================================================================
    D3DXMATRIX RotationY(float angle)
    {
        D3DXMATRIX result;

        D3DXMatrixRotationY(&result, angle);

        return result;
    }

    //=========================================================================
    // Z軸回転
    //=========================================================================
    D3DXMATRIX RotationZ(float angle)
    {
        D3DXMATRIX result;

        D3DXMatrixRotationZ(&result, angle);

        return result;
    }

    //=========================================================================
    // XYZ回転
    //=========================================================================
    D3DXMATRIX Rotation(float x, float y, float z)
    {
        return Rotation(D3DXVECTOR3(x, y, z));
    }

    D3DXMATRIX Rotation(const D3DXVECTOR3& rotation)
    {
        D3DXMATRIX rx = RotationX(rotation.x);
        D3DXMATRIX ry = RotationY(rotation.y);
        D3DXMATRIX rz = RotationZ(rotation.z);

        return rx * ry * rz;
    }

    //=========================================================================
    // クオータニオンから回転行列を作成
    //=========================================================================
    D3DXMATRIX QuaternionToMatrix(const D3DXQUATERNION& quaternion)
    {
        D3DXMATRIX matrix;

        D3DXMatrixRotationQuaternion(&matrix, &quaternion);

        return matrix;
    }

    //=========================================================================
    // 回転行列からクォータニオンを作成
    //=========================================================================
    D3DXQUATERNION MatrixToQuaternion(const D3DXMATRIX mtxRot)
    {
        D3DXQUATERNION quat;
        D3DXQuaternionRotationMatrix(&quat, &mtxRot);
        return quat;
    }

    //=========================================================================
    // 拡縮
    //=========================================================================
    D3DXMATRIX Scaling(float x, float y, float z)
    {
        D3DXMATRIX result;

        D3DXMatrixScaling(&result, x, y, z);

        return result;
    }

    D3DXMATRIX Scaling(const D3DXVECTOR3& scale)
    {
        return Scaling(scale.x, scale.y, scale.z);
    }

    //=========================================================================
    // World行列
    //=========================================================================
    D3DXMATRIX World(
        const D3DXVECTOR3& position,
        const D3DXVECTOR3& rotation,
        const D3DXVECTOR3& scale)
    {
        D3DXMATRIX scaleMatrix = Scaling(scale);

        D3DXMATRIX rotationMatrix = Rotation(rotation);

        D3DXMATRIX translationMatrix = Translation(position);

        return scaleMatrix * rotationMatrix * translationMatrix;
    }

    //=========================================================================
    // World行列 (Quaternion)
    //=========================================================================
    D3DXMATRIX QuatWorld(const D3DXVECTOR3& position, const D3DXQUATERNION& quat)
    {
        D3DXMATRIX mtxWorld = INIT_MTX;
        D3DXMATRIX mtxTrans, mtxRot;

        mtxTrans    = Translation(position);
        mtxRot      = QuaternionToMatrix(quat);
        mtxWorld    = Multiply(mtxWorld, mtxRot);
        mtxWorld    = Multiply(mtxWorld, mtxTrans);

        return mtxWorld;
    }

    //=========================================================================
    // View行列
    //=========================================================================
    D3DXMATRIX LookAtLH(const D3DXVECTOR3& eye, const D3DXVECTOR3& target, const D3DXVECTOR3& up)
    {
        D3DXMATRIX result;

        D3DXMatrixLookAtLH(
            &result,
            &eye,
            &target,
            &up
        );

        return result;
    }

    //=========================================================================
    // Projection行列
    //=========================================================================
    D3DXMATRIX PerspectiveFovLH(float fov, float aspect, float nearZ, float farZ)
    {
        D3DXMATRIX result;

        D3DXMatrixPerspectiveFovLH(
            &result,
            fov,
            aspect,
            nearZ,
            farZ
        );

        return result;
    }

    //=========================================================================
    // 正射影行列
    //=========================================================================
    D3DXMATRIX OrthographicLH(float width, float height, float nearZ, float farZ)
    {
        D3DXMATRIX result;

        D3DXMatrixOrthoLH(
            &result,
            width,
            height,
            nearZ,
            farZ
        );

        return result;
    }

    //=========================================================================
    // 転置
    //=========================================================================
    D3DXMATRIX Transpose(const D3DXMATRIX& matrix)
    {
        D3DXMATRIX result;

        D3DXMatrixTranspose(&result, &matrix);

        return result;
    }

    //=========================================================================
    // 逆行列
    //=========================================================================
    D3DXMATRIX Inverse(const D3DXMATRIX& matrix)
    {
        D3DXMATRIX result;

        D3DXMatrixInverse(&result, nullptr, &matrix);

        return result;
    }

    //=========================================================================
    // 行列の乗算
    //=========================================================================
    D3DXMATRIX Multiply(const D3DXMATRIX& a, const D3DXMATRIX& b)
    {
        return a * b;
    }

    //=========================================================================
    // 座標変換
    //=========================================================================
    D3DXVECTOR3 TransformPoint(const D3DXVECTOR3& position, const D3DXMATRIX& matrix)
    {
        D3DXVECTOR3 result;

        D3DXVec3TransformCoord(&result, &position, &matrix);

        return result;
    }

    //=========================================================================
    // 方向ベクトル変換
    //=========================================================================
    D3DXVECTOR3 TransformDirection(const D3DXVECTOR3& direction, const D3DXMATRIX& matrix)
    {
        D3DXVECTOR3 result;

        D3DXVec3TransformNormal(&result, &direction, &matrix);

        return result;
    }

    //=========================================================================
    // ローカルからワールド
    //=========================================================================
    D3DXVECTOR3 LocalToWorld(
        const D3DXVECTOR3& localPosition,
        const D3DXVECTOR3& position,
        const D3DXVECTOR3& rotation,
        const D3DXVECTOR3& scale)
    {
        D3DXMATRIX worldMatrix = World(
            position,
            rotation,
            scale
        );

        return TransformPoint(localPosition, worldMatrix);
    }

    //=========================================================================
    // 位置取得
    //=========================================================================
    D3DXVECTOR3 GetPosition(const D3DXMATRIX& matrix)
    {
        return D3DXVECTOR3(matrix._41, matrix._42, matrix._43);
    }

    //=========================================================================
    // 回転部分だけ取得
    //=========================================================================
    D3DXMATRIX GetRotationMatrix(const D3DXMATRIX& matrix)
    {
        D3DXMATRIX rotation;

        rotation = matrix;

        // Translationを削除
        rotation._41 = 0.0f;
        rotation._42 = 0.0f;
        rotation._43 = 0.0f;

        return rotation;
    }

    //=========================================================================
    // 回転行列をQuaternionに変換
    //=========================================================================
    D3DXQUATERNION GetRotationQuaternion(const D3DXMATRIX& matrix)
    {
        D3DXQUATERNION quat;
        D3DXQuaternionRotationMatrix(&quat, &matrix);
        return quat;
    }

    //=========================================================================
    // Scale取得
    //=========================================================================
    D3DXVECTOR3 GetScale(const D3DXMATRIX& matrix)
    {
        D3DXVECTOR3 right   (matrix._11, matrix._12, matrix._13);
        D3DXVECTOR3 up      (matrix._21, matrix._22, matrix._23);
        D3DXVECTOR3 forward (matrix._31, matrix._32, matrix._33);

        return D3DXVECTOR3(
            D3DXVec3Length(&right),
            D3DXVec3Length(&up),
            D3DXVec3Length(&forward)
        );
    }

    //=========================================================================
    // Right方向取得
    //=========================================================================
    D3DXVECTOR3 GetRight(const D3DXMATRIX& matrix)
    {
        return D3DXVECTOR3(matrix._11, matrix._12, matrix._13);
    }

    //=========================================================================
    // Up方向取得
    //=========================================================================
    D3DXVECTOR3 GetUp(const D3DXMATRIX& matrix)
    {
        return D3DXVECTOR3(matrix._21, matrix._22, matrix._23);
    }

    //=========================================================================
    // Forward方向取得(デフォルト-Z方向)
    //=========================================================================
    D3DXVECTOR3 GetForward(const D3DXMATRIX& matrix)
    {
        return D3DXVECTOR3(-matrix._31, matrix._32, -matrix._33);
    }

    //=========================================================================
    // クォータニオンの球面線形補間処理
    //=========================================================================
    D3DXQUATERNION QuatSlerp(D3DXQUATERNION quatNow, D3DXQUATERNION quatNext, float fRate)
    {
        // 出力変数
        D3DXQUATERNION QuatOut = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);

        // クォータニオン同士の内積を取る
        float fDotQuat = quatNext.x * quatNow.x + quatNext.y * quatNow.y + quatNext.z * quatNow.z + quatNext.w * quatNow.w;
        fDotQuat = MathUtil::Clamp(fDotQuat, 0.0f, 1.0f);

        float fCosTheta = acosf(fDotQuat);

        // 近いほうから回るようにする
        if (fCosTheta <= 0.0f)
        {
            fCosTheta = -fCosTheta;
        }

        float fSin = sinf(fCosTheta);

        // ゼロ以下だった場合線形補間で返す
        if (fSin <= FLT_EPSILON)
        {
            QuatOut = quatNow + (quatNext - quatNow) * fRate;
            return QuatOut;
        }

        float fSinA = sinf(fCosTheta * (1.0f - fRate));
        float fSinB = sinf(fCosTheta * fRate);

        float fSinMugA = fSinA / fSin;
        float fSinMugB = fSinB / fSin;

        QuatOut.x = quatNow.x * fSinMugA + quatNext.x * fSinMugB;
        QuatOut.y = quatNow.y * fSinMugA + quatNext.y * fSinMugB;
        QuatOut.z = quatNow.z * fSinMugA + quatNext.z * fSinMugB;
        QuatOut.w = quatNow.w * fSinMugA + quatNext.w * fSinMugB;
        return QuatOut;
    }

    //=========================================================================
    // ボーン追従・揺れ処理
    //=========================================================================
    D3DXMATRIX MatrixFollowShaking(
        const D3DXMATRIX& MtxNowOld,
        const D3DXMATRIX& MtxParent,
        const D3DXMATRIX& MtxBoneInit,
        const D3DXMATRIX& MtxParentInit,
        float fShakeRate)
    {
        //------------------------------------------------------------------------
        // 親ボーンの初期状態から現在までの変化量を求める
        //------------------------------------------------------------------------

        D3DXMATRIX MtxParentInitInv;
        D3DXMatrixInverse(&MtxParentInitInv, nullptr, &MtxParentInit);

        // 親ボーンの現在の変化量
        D3DXMATRIX MtxParentDelta = Multiply(MtxParent, MtxParentInitInv);

        //------------------------------------------------------------------------
        // 追従率
        //------------------------------------------------------------------------

        fShakeRate = MathUtil::Clamp(fShakeRate, 0.0f, 1.0f);

        //------------------------------------------------------------------------
        // 親の変化を追従率に応じて適用
        //------------------------------------------------------------------------

        D3DXMATRIX MtxResult = MtxBoneInit;

        // 親の移動量
        D3DXVECTOR3 ParentPos;
        D3DXVECTOR3 ParentInitPos;

        ParentPos.x = MtxParentDelta._41;
        ParentPos.y = MtxParentDelta._42;
        ParentPos.z = MtxParentDelta._43;

        ParentInitPos = INIT_VEC3;

        D3DXVECTOR3 Offset = ParentPos - ParentInitPos;

        Offset *= fShakeRate;

        // 子ボーンに追従移動を加える
        MtxResult._41 += Offset.x;
        MtxResult._42 += Offset.y;
        MtxResult._43 += Offset.z;

        //------------------------------------------------------------------------
        // 前フレームとの補間
        //------------------------------------------------------------------------

        MtxResult._41 = MtxNowOld._41 * (1.0f - fShakeRate) + MtxResult._41 * fShakeRate;
        MtxResult._42 = MtxNowOld._42 * (1.0f - fShakeRate) + MtxResult._42 * fShakeRate;
        MtxResult._43 = MtxNowOld._43 * (1.0f - fShakeRate) + MtxResult._43 * fShakeRate;

        return MtxResult;
    }
}
