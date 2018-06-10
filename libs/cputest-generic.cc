
namespace ZitaResampler {

TARGET_ATTRIBUTE
void CPUTEST_FUNCTION (float *dummy)
{
    PRAGMA_OMP("omp simd")
    for (int i = 0; i < FLOAT_VECTOR_SIZE; i++)
        dummy[i] *= dummy[i];
}

}  // namespace ZitaResampler
