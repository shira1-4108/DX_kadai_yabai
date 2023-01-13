// �s�N�Z���V�F�[�_�[�̃��C���֐��@���@�|���S����`�悷��̂ɕK�v�ȃs�N�Z���������Ăяo�����
// �e�s�N�Z���̐F���w�肷��̂����
// �߂�l�F�@���̃s�N�Z�������F�ɂ��������ARGBA�Ŏw�肷��
// ����inputPos�F�@�s�N�Z���̍��W�B�E�C���h�E�̍�������_�Ƃ���s�N�Z�����W�B

// �e�N�X�`�����󂯎��O���[�o���ϐ�
Texture2D gTexture : register(t0);

// �T���v���[�̃O���[�o���ϐ�
SamplerState gSampler : register(s0);

// �萔�o�b�t�@
#include "cb.hlsl"

float4 ps_main(float4 inputPos : SV_POSITION,
	float2 uv : TEXCOORD, float4 col : COLOR, float3 normal : NORMAL) : SV_Target
{
	// �e�N�X�`���̃e�N�Z���F���o�̓s�N�Z���F�ɂ���
	float4 pixelColor = gTexture.Sample(gSampler, uv);

	// �f�B�t���[�Y�}�e���A���̐F��������
	pixelColor *= gDiffuse;

	// ���C�g�̋������O�����C�g�����s�v
	if (gLightPower <= 0.0f)
	{
		// ���S���߃s�N�Z���Ȃ�o�͂��Ȃ���Z�o�b�t�@���X�V����Ȃ�
		if (pixelColor.a == 0.0f)
			discard;

		return pixelColor;
	}


	//���s�����̏���
	float4 finalColor = pixelColor;
	float3 normalVec = normalize(normal);	//���K�����ꂽ�@���x�N�g��				
	float3 lightVec = normalize(gLightVector);	//���K�����ꂽ���̖@���x�N�g��

	//���̕����x�N�g���Ɩ@���x�N�g���̓��ς��v�Z����
	float brightness = dot(lightVec, normalVec);

	//���܂������邳���s�N�Z���F�ɂ�����
	finalColor = pixelColor * brightness;
	//�}�C�i�X�ňÂ��Ȃ肷�����ꍇ��0�ɖ߂�
	finalColor = saturate(finalColor);
	//�A���t�@�̒l�������ɖ߂�
	finalColor.a = pixelColor.a;

	//�����̏���
	//�ŏI�I�ȐF = ���s�����v�Z�ςݐF + �����v�Z�ςݐF
	//�����v�Z�ςݐF = ���� * ���C�e�B���O�O�̐F
	float4 ambient = gAmbient * pixelColor;
	finalColor = finalColor + ambient;

	return finalColor;
}
