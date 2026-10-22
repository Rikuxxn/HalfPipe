//=============================================================================
//
// 自動移動コンポーネント処理 [AutoMoveComponent.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "AutoMoveComponent.h"
#include "imguimaneger.h"
#include "Manager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "FileDialogUtils.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include "RenderComponent.h"
#include "XModelRenderComponent.h"
#include "MathUtility.h"
#include "TimeSystem.h"
#include "JumpTimingRating.h"


// 名前空間の使用
using namespace MathUtil;


//=============================================================================
// コンストラクタ
//=============================================================================
CAutoMoveComponent::CAutoMoveComponent() :
    m_IsAutoMove			(false),					// 自動移動中か
	m_JumpStartHandled		(false),					// ジャンプ中か
	m_IsTimingActive		(false),					// タイミング判定中か
	m_IsWaitingForJumpPoint	(false),					// ジャンプポイントへの分岐待ちか
	m_HasTimingResult		(false),					// タイミング結果が確定しているか
    m_CurrentMovePoint		(1),						// 現在目指しているポイント
	m_CurrentMovePointMinas	(0),						// 現在目指しているポイント
	m_CurrentMovePointPlus	(2),						// 現在目指しているポイント
	m_CurrentMovePointPlusPlus(3),						// 現在目指しているポイント
	m_JumpStartPoint		(0),						// ジャンプ開始ポイント
	m_JumpTargetPoint		(0),						// ジャンプ先ポイント
	m_currentPoint			(0),						// 現在の移動ポイント(エディタ用)
    m_CurrentTime			(0.0f),						// 現在の移動時間(0.0f～1.0f)
    m_AutoMoveSpeed			(250.0f),					// 移動スピード
    m_TimingResult			(CJumpTiming::TIMING_MAX)   // 確定したタイミング結果
{

}

//=============================================================================
// デストラクタ
//=============================================================================
CAutoMoveComponent::~CAutoMoveComponent()
{

}

//=============================================================================
// 更新
//=============================================================================
void CAutoMoveComponent::Update(void)
{
	// デルタタイムの取得
	float deltaTime = CManager::Instance()->GetTimeSystem()->GetDeltaTime();

	// 自動移動を継続
	if (m_IsAutoMove)
	{
		UpdateAutoMove(deltaTime);
	}

	// タイミング判定結果の取得
	if (m_IsTimingActive)
	{
		UpdateJump(deltaTime);
	}
}

//=============================================================================
// 自動移動ポイントの追加
//=============================================================================
void CAutoMoveComponent::AddMovePoint(const D3DXVECTOR3& position, MovePointType type)
{
	MovePoint point;

	point.Position = position;
	point.Type = type;

	m_MovePoints.push_back(point);

#ifdef _DEBUG
	// デバッグ用自動移動ポイントモデルの生成
	CreateMovePointObject(position, type);
#endif
}

//=============================================================================
// 自動移動ポイントのクリア
//=============================================================================
void CAutoMoveComponent::ClearMovePoints(void)
{
    m_MovePoints.clear();
}

//=============================================================================
// 移動ポイントに沿った自動移動
//=============================================================================
void CAutoMoveComponent::UpdateAutoMove(float deltaTime)
{
	if (!m_IsAutoMove)
	{
		return;
	}

	if (!m_pOwner)
	{
		return;
	}

	// スプライン補間最低4点
	if (m_MovePoints.size() < 4)
	{
		StopAutoMove();
		return;
	}

	// コンポーネントの取得
	auto transform = m_pOwner->GetComponent<CTransformComponent>();
	auto rigid = m_pOwner->GetComponent<CRigidBodyComponent>();

	if (!transform || !rigid)
	{
		return;
	}

	// 現在のポイント番号が有効か確認
	if (m_CurrentMovePoint < 1 ||
		m_CurrentMovePoint + 2 >= m_MovePoints.size()) 
	{
		StopAutoMove();
		return;
	}

	// 現在の移動ポイント
	MovePoint& currentPoint = m_MovePoints[m_CurrentMovePoint];

	//=========================================================
	// ジャンプ開始ポイントの判定
	//=========================================================
	if (currentPoint.Type == MovePointType::JumpStart)
	{
		// まだ判定を開始していない場合
		if (!m_JumpStartHandled)
		{
			// シーンの取得
			auto scene = CManager::Instance()->GetSceneManager()->GetScene();

			if (!scene)
			{
				return;
			}

			// ジャンプタイミングオブジェクトの取得
			auto jumpTiming = scene->FindGameObject<CJumpTiming>("jumpTiming");

			if (!jumpTiming)
			{
				return;
			}

			// ジャンプ開始ポイントを保存
			m_JumpStartPoint = m_CurrentMovePoint;

			// 判定結果をリセット
			m_HasTimingResult = false;
			m_TimingResult = CJumpTiming::TIMING_MAX;

			// タイミング判定を開始
			jumpTiming->StartTiming();

			m_IsTimingActive = true;
			m_IsWaitingForJumpPoint = true;

			// 判定開始済みにする
			m_JumpStartHandled = true;
		}
	}
	else
	{
		// JumpStart以外のポイントに移動した
		m_JumpStartHandled = false;
	}

	//=========================================================
	// ジャンプ分岐地点の判定
	//=========================================================
	if (currentPoint.Type == MovePointType::JumpBranch&& m_IsWaitingForJumpPoint==true)
	{
		// タイミング判定結果がなければBADにする
		if (!m_HasTimingResult)
		{
			m_TimingResult = CJumpTiming::TIMING_BAD;
			m_HasTimingResult = true;
		}

		// シーンの取得
		auto scene = CManager::Instance()->GetSceneManager()->GetScene();

		if (!scene)
		{
			return;
		}

		// ジャンプタイミングオブジェクトの取得
		auto jumpTiming = scene->FindGameObject<CJumpTiming>("jumpTiming");

		if (!jumpTiming)
		{
			return;
		}

		// タイミング判定終了処理
		jumpTiming->EndTiming();

		// 判定結果から分岐先の種類を決定
		MovePointType targetType = GetJumpTargetType(m_TimingResult);

		// ジャンプ開始ポイントより後ろから検索
		for (size_t i = m_JumpStartPoint + 1; i < m_MovePoints.size(); ++i)
		{
			if (m_MovePoints[i].Type == targetType)
			{
				m_JumpTargetPoint = i;

				// 選択したルートに切り替える
				//m_CurrentMovePoint = i;
				m_CurrentMovePointPlus = i;
				m_CurrentMovePointPlusPlus += 3;

				m_CurrentTime = 0.0f;

				// 判定結果を消費
				m_HasTimingResult = false;
				m_IsWaitingForJumpPoint = false;

				return;
			}
		}

		// 対応するジャンプポイントが見つからなかった場合
		StopAutoMove();
		return;
	}


	//=========================================================
	// 通常の自動移動
	//=========================================================

	// 現在位置
	D3DXVECTOR3 position = transform->Position;

	// スプライン補間用のポイント
	const D3DXVECTOR3& p0 = m_MovePoints[m_CurrentMovePointMinas].Position;
	const D3DXVECTOR3& p1 = m_MovePoints[m_CurrentMovePoint].Position;
	const D3DXVECTOR3& p2 = m_MovePoints[m_CurrentMovePointPlus].Position;
	const D3DXVECTOR3& p3 = m_MovePoints[m_CurrentMovePointPlusPlus].Position;


	// 目標位置
	D3DXVECTOR3 target = MathUtil::Spline(
		p0, p1, p2, p3, m_CurrentTime);

	// 目標への方向
	D3DXVECTOR3 direction = target - position;

	// スプラインの区間が終了した
	if (m_CurrentTime >= 1.0f)
	{
		//++m_CurrentMovePoint;
		m_CurrentMovePointMinas = m_CurrentMovePoint;
		m_CurrentMovePoint = m_CurrentMovePointPlus;
		m_CurrentMovePointPlus = m_CurrentMovePointPlusPlus;
		++m_CurrentMovePointPlusPlus;
		m_CurrentTime = 0.0f;
		return;
	}

	// 時間を進める
	m_CurrentTime += 0.01f;

	// 正規化
	D3DXVec3Normalize(&direction, &direction);

	// 目標速度
	D3DXVECTOR3 velocity = direction /** m_AutoMoveSpeed*/;

	// 物理速度を設定
	rigid->SetPhysicsVelocity(velocity);

	transform->Position = target;

	rigid->Transform();
}

//=============================================================================
// ジャンプ更新
//=============================================================================
void CAutoMoveComponent::UpdateJump(float deltaTime)
{
	// 判定中でなければ処理しない
	if (!m_IsTimingActive)
	{
		return;
	}

	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (!scene)
	{
		return;
	}

	// ジャンプタイミングオブジェクトの取得
	auto jumpTiming = scene->FindGameObject<CJumpTiming>("jumpTiming");

	if (!jumpTiming)
	{
		return;
	}

	// 判定結果を取得
	CJumpTiming::TIMING timing;

	if (jumpTiming->ConsumeTimingResult(timing))
	{
		// 判定結果を保存
		m_TimingResult = timing;
		m_HasTimingResult = true;

		// タイミング判定終了
		m_IsTimingActive = false;
	}

	//// BAD判定
	//OnTimingResult(CJumpTiming::TIMING_BAD);
}

//=============================================================================
// タイミング結果から移動先の種類を取得
//=============================================================================
MovePointType CAutoMoveComponent::GetJumpTargetType(CJumpTiming::TIMING timing)
{
	switch (timing)
	{
	case CJumpTiming::TIMING_PERFECT:
		return MovePointType::JumpExtraLarge;

	case CJumpTiming::TIMING_GREAT:
		return MovePointType::JumpLarge;

	case CJumpTiming::TIMING_GOOD:
		return MovePointType::JumpMedium;

	case CJumpTiming::TIMING_BAD:
		return MovePointType::JumpSmall;

	default:
		return MovePointType::Normal;
	}
}

//=============================================================================
// ポイントデバッグ表示用モデル生成
//=============================================================================
void CAutoMoveComponent::CreateMovePointObject(const D3DXVECTOR3& pos, MovePointType type)
{
	// シーンの取得
	auto scene = CManager::Instance()->GetSceneManager()->GetScene();

	if (scene)
	{
		// ゲームオブジェクトの生成
		auto point = scene->CreateObject();

		// タグの設定
		point->SetTag("MovePoint");

		// トランスフォームコンポーネントの追加
		auto transform = point->AddComponent<CTransformComponent>();

		transform->Position = pos;

		//-----------------------------------------
		// モデル描画
		//-----------------------------------------

		// モデルレンダーコンポーネントの追加
		auto model = point->AddComponent<CXModelRenderComponent>();

		// 描画順の設定
		model->SetLayer(RenderLayer::Model);

		switch (type)
		{
		case MovePointType::Normal:
		{
			// モデルのロード
			model->Load("data/MODELS/MovePoint.x");
			break;
		}
		case MovePointType::JumpStart:
		{
			// モデルのロード
			model->Load("data/MODELS/JumpStartPoint.x");
			break;
		}
		case MovePointType::JumpBranch:
		{
			// モデルのロード
			model->Load("data/MODELS/rock_01.x");
			break;
		}
		case MovePointType::JumpExtraLarge:
		{
			// モデルのロード
			model->Load("data/MODELS/JumpExtraLarge.x");
			break;
		}
		case MovePointType::JumpLarge:
		{
			// モデルのロード
			model->Load("data/MODELS/JumpLarge.x");
			break;
		}
		case MovePointType::JumpMedium:
		{
			// モデルのロード
			model->Load("data/MODELS/JumpMedium.x");
			break;
		}
		case MovePointType::JumpSmall:
		{
			// モデルのロード
			model->Load("data/MODELS/JumpSmall.x");
			break;
		}
		}

		// ゲームオブジェクトの追加
		m_pPointObj.push_back(point);
	}
}

//=============================================================================
// ポイントの種類に応じてモデルを変更
//=============================================================================
void CAutoMoveComponent::UpdateMovePointModel(size_t index)
{
	// インデックスの確認
	if (index >= m_MovePoints.size() ||
		index >= m_pPointObj.size())
	{
		return;
	}

	auto pointObj = m_pPointObj[index];

	if (!pointObj)
	{
		return;
	}

	// モデルコンポーネントの取得
	auto model = pointObj->GetComponent<CXModelRenderComponent>();

	if (!model)
	{
		return;
	}

	// ポイントの種類に応じてモデルを変更
	switch (m_MovePoints[index].Type)
	{
	case MovePointType::Normal:
	{
		// モデルのロード
		model->Load("data/MODELS/MovePoint.x");
		break;
	}
	case MovePointType::JumpStart:
	{
		// モデルのロード
		model->Load("data/MODELS/JumpStartPoint.x");
		break;
	}
	case MovePointType::JumpBranch:
	{
		// モデルのロード
		model->Load("data/MODELS/rock_01.x");
		break;
	}
	case MovePointType::JumpExtraLarge:
	{
		// モデルのロード
		model->Load("data/MODELS/JumpExtraLarge.x");
		break;
	}
	case MovePointType::JumpLarge:
	{
		// モデルのロード
		model->Load("data/MODELS/JumpLarge.x");
		break;
	}
	case MovePointType::JumpMedium:
	{
		// モデルのロード
		model->Load("data/MODELS/JumpMedium.x");
		break;
	}
	case MovePointType::JumpSmall:
	{
		// モデルのロード
		model->Load("data/MODELS/JumpSmall.x");
		break;
	}
	}
}

//=============================================================================
// 自動移動ポイントエディターの描画
//=============================================================================
void CAutoMoveComponent::DrawMovePointEditor(void)
{
	// プレイヤー自動移動ポイント設置エディター
	if (ImGui::TreeNode("MovePoint Editor"))
	{
		//=========================================================
		// ポイントの種類
		//=========================================================

		static int pointType = static_cast<int>(MovePointType::Normal);

		const char* pointTypeNames[] =
		{
			"Normal",
			"Jump Start",
			"Jump Branch",
			"Jump Extra Large",
			"Jump Large",
			"Jump Medium",
			"Jump Small"
		};

		ImGui::Combo(
			"Point Type",
			&pointType,
			pointTypeNames,
			IM_ARRAYSIZE(pointTypeNames));

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		//=========================================================
		// 自動移動ポイントの追加
		//=========================================================

		static float addPos[3] = { 0.0f,0.0f,0.0f };

		ImGui::DragFloat3("Add Point", addPos, 1.0f);

		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		if (ImGui::Button("Add Move Point"))
		{
			AddMovePoint(
				D3DXVECTOR3(addPos[0], addPos[1], addPos[2]),
				static_cast<MovePointType>(pointType));
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		//=========================================================
		// 自動移動ポイントリスト
		//=========================================================

		for (int i = 0; i < (int)m_MovePoints.size(); i++)
		{
			ImGui::PushID(i);

			MovePoint& point = m_MovePoints[i];

			char label[64];

			sprintf_s(label, "Point %d", i);

			if (ImGui::TreeNode(label))
			{
				//=================================================
				// ポイントの種類
				//=================================================

				int type = static_cast<int>(point.Type);

				if (ImGui::Combo(
					"Type",
					&type,
					pointTypeNames,
					IM_ARRAYSIZE(pointTypeNames)))
				{
					point.Type = static_cast<MovePointType>(type);
					
					// 対応するモデルも変更
					UpdateMovePointModel(i);
				}

				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				//=================================================
				// 位置
				//=================================================

				if (ImGui::DragFloat3("Pos", &point.Position.x, 1.0f))
				{
					if (i < m_pPointObj.size() && m_pPointObj[i])
					{
						auto pointTransform =
							m_pPointObj[i]->GetComponent<CTransformComponent>();

						if (pointTransform)
						{
							pointTransform->Position = point.Position;
						}
					}
				}

				ImGui::Dummy(ImVec2(0.0f, 5.0f));

				//=================================================
				// 削除
				//=================================================

				if (ImGui::Button("Delete Point"))
				{
					// モデル削除
					if (i < m_pPointObj.size())
					{
						if (m_pPointObj[i])
						{
							m_pPointObj[i]->Destroy();
						}

						m_pPointObj.erase(m_pPointObj.begin() + i);
					}

					m_MovePoints.erase(m_MovePoints.begin() + i);

					// currentPoint補正
					if (m_currentPoint >= (int)m_MovePoints.size())
					{
						m_currentPoint = std::max(0, (int)m_MovePoints.size() - 1);
					}

					ImGui::TreePop();
					ImGui::PopID();

					break;
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		if (ImGui::Button("Save"))
		{
			// ダイアログを開いてファイルに保存
			std::string path = OpenWindowsSaveFileDialog();

			if (!path.empty())
			{
				// データの保存
				SaveMovePoint(path.c_str());
			}
		}

		ImGui::SameLine(0);

		if (ImGui::Button("Load"))
		{
			// ダイアログを開いてファイルを開く
			std::string path = OpenWindowsOpenFileDialog();

			if (!path.empty())
			{
				// データの読み込み
				LoadMovePoint(path.c_str());
			}
		}

		ImGui::TreePop(); // 閉じる
	}
}

//=============================================================================
// JSONでの保存
//=============================================================================
void CAutoMoveComponent::SaveMovePoint(const char* filename)
{
	// JSONオブジェクト
	json j;
	json b;
	json patrolArray = json::array();

	for (const auto& point : m_MovePoints)
	{
		json p;

		// 座標
		p["position"] =
		{
			point.Position.x,
			point.Position.y,
			point.Position.z
		};

		// 移動ポイントの種類
		p["type"] = static_cast<int>(point.Type);

		patrolArray.push_back(p);
	}

	b["move_points"] = patrolArray;

	// 追加
	j.push_back(b);

	// 出力ファイルストリーム
	std::ofstream file(filename);

	if (file.is_open())
	{
		file << std::setw(4) << j;

		// ファイルを閉じる
		file.close();
	}
}

//=============================================================================
// JSONでの読み込み
//=============================================================================
void CAutoMoveComponent::LoadMovePoint(const char* filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{// 開けなかった
		return;
	}

	json j;
	file >> j;

	// ファイルを閉じる
	file.close();

	// すべて削除
	for (auto obj : m_pPointObj)
	{
		if (obj)
		{
			obj->Destroy();
		}
	}

	m_pPointObj.clear();
	m_MovePoints.clear();

	// JSONから読み込み
	for (const auto& b : j)
	{
		if (!b.contains("move_points") || !b["move_points"].is_array())
		{
			continue;
		}

		for (const auto& p : b["move_points"])
		{
			// 座標が存在するか確認
			if (!p.contains("position") ||
				!p["position"].is_array() ||
				p["position"].size() < 3)
			{
				continue;
			}

			// 座標を取得
			D3DXVECTOR3 position(
				p["position"][0].get<float>(),
				p["position"][1].get<float>(),
				p["position"][2].get<float>()
			);

			// ポイントの種類を取得
			MovePointType type = MovePointType::Normal;

			if (p.contains("type") && p["type"].is_number_integer())
			{
				int typeValue = p["type"].get<int>();

				// 有効な範囲か確認
				if (typeValue >= static_cast<int>(MovePointType::Normal) &&
					typeValue <= static_cast<int>(MovePointType::JumpSmall))
				{
					type = static_cast<MovePointType>(typeValue);
				}
			}

			// ポイントを追加
			MovePoint point;
			point.Position = position;
			point.Type = type;

			m_MovePoints.push_back(point);

#ifdef _DEBUG
			// ポイントデバッグ表示用モデルの生成
			CreateMovePointObject(position, type);
#endif
		}
	}
}
