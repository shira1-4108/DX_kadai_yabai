

// C++����萔�o�b�t�@�œ]�����ꂽ�f�[�^���Z�b�g�����
cbuffer ConstantBufferMatrix : register(b1)
{
	matrix gWorld;		// ���[���h�ϊ��s��
	matrix gView;		// �r���[�ϊ��s��
	matrix gProjection;	// �v���W�F�N�V�����ϊ��s��
	matrix gWorldRotate;// ��]�s��
	float4 gUvOffset;	// UV�A�j���[�V�����p
	float4 gDiffuse;	// �}�e���A�����\�ʂ̐F
	float3 gLightVector;// ���̌���
	float gLightPower;	// ���̋���
	float4 gAmbient;	// �����̐F�Ƌ���
}

